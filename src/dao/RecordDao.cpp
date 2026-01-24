#include "RecordDao.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

//add借出记录
bool RecordDao::addBorrowRecord(QSqlDatabase& db, const QString& userId, const QString& gearId) {
    QSqlQuery query(db);
    query.prepare(QStringLiteral("INSERT INTO record (user_id, gear_id, borrow_time, cost) VALUES (?, ?, NOW(), 0.0)")); //NOW()表示当前时间
    query.addBindValue(userId);
    query.addBindValue(gearId);

    if (!query.exec()) {
        qCritical() << "插入借还记录失败:" << query.lastError().text();
        return false;
    }
    return true;
}

//根据ID查找借伞未归还的记录
std::optional<BorrowRecord> RecordDao::selectUnfinishedByUserId(QSqlDatabase& db, const QString& userId) {
    QSqlQuery query(db);
    //return_time IS NULL就是返回时间为空的就是未归还的
    query.prepare(QStringLiteral("SELECT record_id, user_id, gear_id, borrow_time, return_time, cost FROM record WHERE user_id = ? AND return_time IS NULL LIMIT 1"));
    query.addBindValue(userId);

    if (!query.exec()) {
        qCritical() << "查询未归还记录失败:" << query.lastError().text();
        return std::nullopt;
    }
    if (query.next()) {
        return BorrowRecord(query.value("record_id").toLongLong(), query.value("user_id").toString(), query.value("gear_id").toString(), query.value("borrow_time").toDateTime(), query.value("return_time").toDateTime(), query.value("cost").toDouble());
    }
    return std::nullopt;
}

//更新还伞结账信息,这里传入record_id作为参数
bool RecordDao::updateReturnInfo(QSqlDatabase& db, qint64 recordId, const QDateTime& returnTime, double cost) {
    QSqlQuery query(db);
    //更新return_time为传入的时间，写入费用（确保与计费时使用的时间一致）
    query.prepare(QStringLiteral("UPDATE record SET return_time = ?, cost = ? WHERE record_id = ?"));
    query.addBindValue(returnTime);
    query.addBindValue(cost);
    query.addBindValue(recordId);

    if (!query.exec()) {
        qCritical() << "更新还伞记录失败:" << query.lastError().text();
        return false;
    }
    
    // 验证更新是否成功
    if (query.numRowsAffected() == 0) {
        qWarning() << "更新还伞记录：未找到对应的记录ID" << recordId;
        return false;
    }
    
    return true;
}



//管理员后台Part
//获取最近订单
QVector<OrderInfoDTO> RecordDao::selectRecent(QSqlDatabase& db, int limit) {
    QVector<OrderInfoDTO> result;
    QSqlQuery query(db);
    query.prepare(QString("SELECT record_id, user_id, gear_id, borrow_time, return_time, cost FROM record ORDER BY borrow_time DESC LIMIT %1").arg(limit));
    
    if (!query.exec()) { return result; }
    
    while (query.next()) {
        QString returnTime = query.value("return_time").isNull() ? QString() 
            : query.value("return_time").toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        result.append(OrderInfoDTO{
            query.value("record_id").toLongLong(), 
            query.value("user_id").toString(), 
            query.value("gear_id").toString(), 
            query.value("borrow_time").toDateTime().toString("yyyy-MM-dd hh:mm:ss"), 
            returnTime, 
            query.value("cost").toDouble()
        });
    }
    return result;
}