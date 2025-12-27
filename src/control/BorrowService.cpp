#include"BorrowService.h"
#include"../utils/ConnectionPool.h"
#include<QDebug>
#include<QtMath>

//借伞业务逻辑，传入用户ID和雨具ID
ServiceResult BorrowService::borrowGear(const QString& userId, const QString& gearId) {
    auto db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return {false,"数据库连接失败"};

    //检查用户是否存在&激活
    auto userBox=userDao.selectById(db, userId);
    if (!userBox) return {false,"用户不存在"};
    if (!userBox->get_is_active()) return {false, "账户未激活，请先去激活"};
    //检查用户是否已经借伞
    auto unfinishedRecord = recordDao.selectUnfinishedByUserId(db, userId);
    if (unfinishedRecord.has_value()) { return {false, "您有未归还的订单，请先归还后再借"}; }
    
    //检查雨具是否存在
    auto gear = gearDao.selectById(db, gearId);
    if(!gear) return {false,"雨具识别错误(ID不存在)"};
    //检查雨具状态
    if(gear->get_status()!= GearStatus::Available){ return {false,"该雨具已被借出或处于维护中"}; }
    
    //检查余额是否足够支付押金
    double deposit=gear->get_deposit(); 
    if (userBox->get_credit()<deposit){
        return {false, QString("余额不足，当前雨具需押金 %1 元").arg(deposit)};
    }

    if (!db.transaction()) return {false, "事务开启失败"};
    bool success = true;
    if (!userDao.updateBalance(db, userId, -deposit)) {
        qCritical() << "借伞失败：扣款步骤出错";
        success = false;
    }

    //更新雨具状态为借出
    if (success && !gearDao.updateStatusAndLocation(db, gearId, GearStatus::Borrowed, Station::Unknown, 0)) {
        qCritical() << "借伞失败：更新雨具状态出错";
        success = false;
    }
    //插入借出记录 (Record)
    if (success && !recordDao.addBorrowRecord(db, userId, gearId)) {
        qCritical() << "借伞失败：创建订单记录出错";
        success = false;
    }

    if (success) {
        db.commit(); 
        qInfo() <<"用户"<< userId <<"成功借出雨具"<<gearId;
        return {true, "借伞成功！请取走您的雨具"};
    } else {
        db.rollback(); 
        return {false, "系统内部错误，交易已取消"};
    }
}

//还伞业务逻辑，传入用户ID和雨具ID，站点ID和槽位ID
ServiceResult BorrowService::returnGear(const QString& userId, const QString& gearId, Station stationId, int slotId) {
    auto db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return {false, "数据库连接失败"};

    //添加雨具对应槽位的判断
    auto gear = gearDao.selectById(db, gearId);
    if (!gear) return {false, "雨具信息异常(ID不存在)"};
    bool isSlotValid = false;
    GearType type = gear->get_type();
    //根据雨具类型判断槽位是否合法
    switch (type) {
        case GearType::StandardPlastic:   //普通塑料伞: 1-4
            if (slotId >= 1 && slotId <= 4) isSlotValid = true;
            break;
        case GearType::PremiumWindproof:  //高质量抗风伞: 5-8
            if (slotId >= 5 && slotId <= 8) isSlotValid = true;
            break;
        case GearType::Sunshade:          //专用遮阳伞: 9-10
            if (slotId >= 9 && slotId <= 10) isSlotValid = true;
            break;
        case GearType::Raincoat:          //雨衣: 11-12
            if (slotId >= 11 && slotId <= 12) isSlotValid = true;
            break;
        default:
            break;
    }

    if (!isSlotValid) {return {false, "归还位置错误！该类型雨具只能还到指定区域（请查看槽位说明）"};}

    //检查归还的槽位是否已经被占了
    if (gearDao.isSlotOccupied(db, stationId, slotId)) { return {false, "该槽位已有雨具，请更换槽位"}; }

    //查找该用户的未归还订单
    auto recordBox = recordDao.selectUnfinishedByUserId(db, userId);
    if (!recordBox.has_value()) { return {false, "未查询到您的借出记录"}; }
    //校验归还的伞是否和借的一样
    if (recordBox->get_gear_id() != gearId) {
        return {false, "归还的雨具ID与订单不符"};
    }

    if (!gear) return {false,"雨具信息异常"};
    QDateTime borrowTime = recordBox->get_borrow_time();
    QDateTime returnTime = QDateTime::currentDateTime(); 
    
    //计算租金
    double cost = calculateCost(borrowTime, returnTime, gear->get_type());
    double deposit = gear->get_deposit();
    
    //费用最多等于押金，不会倒扣用户余额
    if (cost > deposit) { cost = deposit;}
    
    //应退金额 = 押金 - 费用（最低为0）
    double refund = deposit - cost;
    
    if (!db.transaction()) return {false, "系统忙"};
    bool success = true;
    //填入归还时间和费用
    if (!recordDao.updateReturnInfo(db, recordBox->get_record_id(), cost)) { success = false; }
    //更新雨具状态为可用
    if (success && !gearDao.updateStatusAndLocation(db, gearId, GearStatus::Available, stationId, slotId)) { success = false; }
    //退还押金 (扣除租金后的余额)
    if (success && !userDao.updateBalance(db, userId, refund)) { success = false; }

    if (success) {
        db.commit();
        QString msg = QString("还伞成功！产生费用 %1 元，退回 %2 元").arg(cost, 0, 'f', 2).arg(refund, 0, 'f', 2);
        qInfo() << msg;
        return {true, msg, cost};
    } else {
        db.rollback();
        return {false, "还伞失败，系统回滚"};
    }
}

//辅助函数：计费规则
double BorrowService::calculateCost(const QDateTime& borrowTime, const QDateTime& returnTime, GearType type) {
    //计算秒数差
    qint64 seconds = borrowTime.secsTo(returnTime);
    if (seconds <= 0) return 0.0; 
    //转换成小时 
    double hours = qCeil(seconds / 3600.0); 
    //根据类型定单价
    double rate = 1.0; // 默认 1元/小时
    switch (type) {
        case GearType::StandardPlastic: rate = 1.0; break;
        case GearType::PremiumWindproof: rate = 2.0; break;
        case GearType::Sunshade: rate = 1.5; break;
        case GearType::Raincoat: rate = 2.0; break; 
        default: rate = 1.0; break;
    }
    //计算总价
    double total = hours * rate;
    return total;
}