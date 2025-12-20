#include "DatabaseManager.h"
#include "DBHelper.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QDateTime>

std::optional<DatabaseManager::UserRecord> DatabaseManager::fetchUserByIdAndName(const QString &userId, const QString &realName)
{
    if (userId.trimmed().isEmpty() || realName.trimmed().isEmpty()) {
        return std::nullopt;
    }

    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return std::nullopt;
    }

    QSqlQuery query(db);
    // 查询users表，包含is_active和password字段
    query.prepare(QStringLiteral(
        "SELECT user_id, real_name, credit, role, is_active, password "
        "FROM users WHERE user_id = :uid AND real_name = :name LIMIT 1"));
    query.bindValue(":uid", userId.trimmed());
    query.bindValue(":name", realName.trimmed());

    if (!query.exec()) {
        qWarning() << "[DB] 查询失败:" << query.lastError().text();
        return std::nullopt;
    }

    if (query.next()) {
        UserRecord rec;
        rec.userId = query.value(0).toString();
        rec.realName = query.value(1).toString();
        rec.credit = query.value(2).toDouble();
        rec.role = query.value(3).toInt();
        rec.isActive = query.value(4).toBool();
        rec.password = query.value(5).toString(); // 可能为NULL
        return rec;
    }
    return std::nullopt;
}

std::optional<DatabaseManager::UserRecord> DatabaseManager::fetchUserByIdAndNameAndPassword(const QString &userId, const QString &realName, const QString &plainPassword)
{
    if (userId.trimmed().isEmpty() || realName.trimmed().isEmpty() || plainPassword.isEmpty()) {
        qWarning() << "[DB] 空账号/姓名/密码";
        return std::nullopt;
    }

    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return std::nullopt;
    }

    QSqlQuery query(db);
    // 使用明文密码比较（不使用哈希）
    query.prepare(QStringLiteral(
        "SELECT user_id, real_name, password, credit, role, is_active "
        "FROM users WHERE user_id = :uid AND real_name = :name LIMIT 1"));
    query.bindValue(":uid", userId.trimmed());
    query.bindValue(":name", realName.trimmed());
    
    if (!query.exec()) {
        qWarning() << "[DB] 查询失败:" << query.lastError().text();
        return std::nullopt;
    }
    
    if (query.next()) {
        QString dbPassword = query.value(2).toString(); // 数据库中的密码（明文）
        // 明文密码直接比较
        if (dbPassword == plainPassword) {
            UserRecord rec;
            rec.userId = query.value(0).toString();
            rec.realName = query.value(1).toString();
            rec.password = dbPassword;
            rec.credit = query.value(3).toDouble();
            rec.role = query.value(4).toInt();
            rec.isActive = query.value(5).toBool();
            return rec;
        } else {
            qWarning() << "[DB] 密码不匹配";
            return std::nullopt;
        }
    }
    qWarning() << "[DB] 账号/姓名不存在";
    return std::nullopt;
}

bool DatabaseManager::updateUserPassword(const QString &userId, const QString &newPassword)
{
    if (userId.trimmed().isEmpty() || newPassword.isEmpty()) {
        qWarning() << "[DB] 空账号或密码";
        return false;
    }

    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return false;
    }

    QSqlQuery query(db);
    // 更新密码并设置is_active为1（激活状态）
    query.prepare(QStringLiteral(
        "UPDATE users SET password = :pwd, is_active = 1 WHERE user_id = :uid"));
    query.bindValue(":pwd", newPassword);
    query.bindValue(":uid", userId.trimmed());
    
    if (!query.exec()) {
        qWarning() << "[DB] 更新密码失败:" << query.lastError().text();
        return false;
    }
    
    qInfo() << "[DB] 用户" << userId << "密码已更新，账户已激活";
    return true;
}

bool DatabaseManager::changeUserPassword(const QString &userId, const QString &oldPassword, const QString &newPassword)
{
    if (userId.trimmed().isEmpty() || oldPassword.isEmpty() || newPassword.isEmpty()) {
        qWarning() << "[DB] 空账号或密码";
        return false;
    }

    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return false;
    }

    QSqlQuery query(db);
    // 先验证旧密码
    query.prepare(QStringLiteral(
        "SELECT password FROM users WHERE user_id = :uid LIMIT 1"));
    query.bindValue(":uid", userId.trimmed());
    
    if (!query.exec() || !query.next()) {
        qWarning() << "[DB] 用户不存在";
        return false;
    }
    
    QString dbPassword = query.value(0).toString();
    if (dbPassword != oldPassword) {
        qWarning() << "[DB] 旧密码不正确";
        return false;
    }
    
    // 更新为新密码
    query.prepare(QStringLiteral(
        "UPDATE users SET password = :pwd WHERE user_id = :uid"));
    query.bindValue(":pwd", newPassword);
    query.bindValue(":uid", userId.trimmed());
    
    if (!query.exec()) {
        qWarning() << "[DB] 更新密码失败:" << query.lastError().text();
        return false;
    }
    
    qInfo() << "[DB] 用户" << userId << "密码已修改";
    return true;
}

bool DatabaseManager::init()
{
    // 测试数据库连接
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    return db.isOpen();
}

QString DatabaseManager::connName()
{
    return QStringLiteral("rainhub_main_conn");
}

bool DatabaseManager::ensureConnectionOpen()
{
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    return db.isOpen();
}

QSqlDatabase DatabaseManager::connection()
{
    return DBHelper::getThreadLocalConnection();
}

// ========== 用户余额更新 ==========
bool DatabaseManager::updateUserBalance(const QString &userId, double amount)
{
    if (userId.trimmed().isEmpty()) {
        qWarning() << "[DB] 空账号";
        return false;
    }

    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "UPDATE users SET credit = credit + :amount WHERE user_id = :uid"));
    query.bindValue(":amount", amount);
    query.bindValue(":uid", userId.trimmed());
    
    if (!query.exec()) {
        qWarning() << "[DB] 更新余额失败:" << query.lastError().text();
        return false;
    }
    
    qInfo() << "[DB] 用户" << userId << "余额已更新，变化:" << amount;
    return true;
}

// ========== 站点相关 ==========
QVector<DatabaseManager::StationRecord> DatabaseManager::fetchAllStations()
{
    QVector<StationRecord> stations;
    
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return stations;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT station_id, name, pos_x, pos_y, status FROM station ORDER BY station_id"));
    
    if (!query.exec()) {
        qWarning() << "[DB] 查询站点失败:" << query.lastError().text();
        return stations;
    }

    while (query.next()) {
        StationRecord rec;
        rec.stationId = query.value(0).toInt();
        rec.name = query.value(1).toString();
        rec.posX = query.value(2).toFloat();
        rec.posY = query.value(3).toFloat();
        rec.status = query.value(4).toInt();
        stations.append(rec);
    }
    
    return stations;
}

// ========== 雨具相关 ==========
QVector<DatabaseManager::GearRecord> DatabaseManager::fetchGearsByStation(int stationId)
{
    QVector<GearRecord> gears;
    
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return gears;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT gear_id, type_id, station_id, slot_id, status "
        "FROM raingear WHERE station_id = :sid ORDER BY slot_id"));
    query.bindValue(":sid", stationId);
    
    if (!query.exec()) {
        qWarning() << "[DB] 查询雨具失败:" << query.lastError().text();
        return gears;
    }

    while (query.next()) {
        GearRecord rec;
        rec.gearId = query.value(0).toString();
        rec.typeId = query.value(1).toInt();
        rec.stationId = query.value(2).toInt();
        rec.slotId = query.value(3).toInt();
        rec.status = query.value(4).toInt();
        gears.append(rec);
    }
    
    return gears;
}

QVector<DatabaseManager::GearRecord> DatabaseManager::fetchAllGears()
{
    QVector<GearRecord> gears;
    
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return gears;
    }

    QSqlQuery query(db);
    // 查询所有雨具，包括已借出的（station_id 为 NULL）
    query.prepare(QStringLiteral(
        "SELECT gear_id, type_id, COALESCE(station_id, 0), COALESCE(slot_id, 0), status "
        "FROM raingear ORDER BY gear_id"));
    
    if (!query.exec()) {
        qWarning() << "[DB] 查询所有雨具失败:" << query.lastError().text();
        return gears;
    }

    while (query.next()) {
        GearRecord rec;
        rec.gearId = query.value(0).toString();
        rec.typeId = query.value(1).toInt();
        rec.stationId = query.value(2).toInt();
        rec.slotId = query.value(3).toInt();
        rec.status = query.value(4).toInt();
        gears.append(rec);
    }
    
    return gears;
}

std::optional<DatabaseManager::GearRecord> DatabaseManager::fetchGearById(const QString &gearId)
{
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return std::nullopt;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT gear_id, type_id, station_id, slot_id, status "
        "FROM raingear WHERE gear_id = :gid LIMIT 1"));
    query.bindValue(":gid", gearId);
    
    if (!query.exec()) {
        qWarning() << "[DB] 查询雨具失败:" << query.lastError().text();
        return std::nullopt;
    }

    if (query.next()) {
        GearRecord rec;
        rec.gearId = query.value(0).toString();
        rec.typeId = query.value(1).toInt();
        rec.stationId = query.value(2).toInt();
        rec.slotId = query.value(3).toInt();
        rec.status = query.value(4).toInt();
        return rec;
    }
    
    return std::nullopt;
}

// ========== 雨具管理（管理员功能） ==========
bool DatabaseManager::addGear(const QString &gearId, int typeId, int stationId, int slotId)
{
    if (gearId.trimmed().isEmpty()) {
        qWarning() << "[DB] 雨具ID不能为空";
        return false;
    }
    
    if (typeId < 1 || typeId > 4) {
        qWarning() << "[DB] 雨具类型ID无效:" << typeId;
        return false;
    }
    
    if (slotId < 1 || slotId > 12) {
        qWarning() << "[DB] 槽位ID无效:" << slotId << "，范围应为1-12";
        return false;
    }
    
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return false;
    }
    
    // 开启事务
    if (!db.transaction()) {
        qWarning() << "[DB] 无法开启事务";
        return false;
    }
    
    QSqlQuery query(db);
    
    try {
        // 1. 检查雨具ID是否已存在
        query.prepare(QStringLiteral("SELECT COUNT(*) FROM raingear WHERE gear_id = :gid"));
        query.bindValue(":gid", gearId.trimmed());
        if (!query.exec() || !query.next()) {
            qWarning() << "[DB] 检查雨具ID失败";
            db.rollback();
            return false;
        }
        if (query.value(0).toInt() > 0) {
            qWarning() << "[DB] 雨具ID已存在:" << gearId;
            db.rollback();
            return false;
        }
        
        // 2. 检查站点是否存在
        query.prepare(QStringLiteral("SELECT COUNT(*) FROM station WHERE station_id = :sid"));
        query.bindValue(":sid", stationId);
        if (!query.exec() || !query.next()) {
            qWarning() << "[DB] 检查站点失败";
            db.rollback();
            return false;
        }
        if (query.value(0).toInt() == 0) {
            qWarning() << "[DB] 站点不存在:" << stationId;
            db.rollback();
            return false;
        }
        
        // 3. 检查槽位是否已被占用
        query.prepare(QStringLiteral(
            "SELECT COUNT(*) FROM raingear WHERE station_id = :sid AND slot_id = :slid"));
        query.bindValue(":sid", stationId);
        query.bindValue(":slid", slotId);
        if (!query.exec() || !query.next()) {
            qWarning() << "[DB] 检查槽位失败";
            db.rollback();
            return false;
        }
        if (query.value(0).toInt() > 0) {
            qWarning() << "[DB] 槽位已被占用，station_id:" << stationId << "slot_id:" << slotId;
            db.rollback();
            return false;
        }
        
        // 4. 插入新雨具
        query.prepare(QStringLiteral(
            "INSERT INTO raingear (gear_id, type_id, station_id, slot_id, status) "
            "VALUES (:gid, :tid, :sid, :slid, 1)"));
        query.bindValue(":gid", gearId.trimmed());
        query.bindValue(":tid", typeId);
        query.bindValue(":sid", stationId);
        query.bindValue(":slid", slotId);
        
        if (!query.exec()) {
            qWarning() << "[DB] 插入雨具失败:" << query.lastError().text();
            db.rollback();
            return false;
        }
        
        // 提交事务
        if (!db.commit()) {
            qWarning() << "[DB] 提交事务失败";
            db.rollback();
            return false;
        }
        
        qInfo() << "[DB] 添加雨具成功: gear_id=" << gearId << "type_id=" << typeId 
                << "station_id=" << stationId << "slot_id=" << slotId;
        return true;
        
    } catch (...) {
        db.rollback();
        qWarning() << "[DB] 添加雨具操作异常，已回滚";
        return false;
    }
}

bool DatabaseManager::removeGear(const QString &gearId)
{
    if (gearId.trimmed().isEmpty()) {
        qWarning() << "[DB] 雨具ID不能为空";
        return false;
    }
    
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return false;
    }
    
    // 开启事务
    if (!db.transaction()) {
        qWarning() << "[DB] 无法开启事务";
        return false;
    }
    
    QSqlQuery query(db);
    
    try {
        // 1. 检查雨具是否存在
        query.prepare(QStringLiteral("SELECT status FROM raingear WHERE gear_id = :gid LIMIT 1"));
        query.bindValue(":gid", gearId.trimmed());
        if (!query.exec() || !query.next()) {
            qWarning() << "[DB] 雨具不存在:" << gearId;
            db.rollback();
            return false;
        }
        
        int status = query.value(0).toInt();
        if (status == 2) { // 已借出
            qWarning() << "[DB] 无法删除已借出的雨具:" << gearId;
            db.rollback();
            return false;
        }
        
        // 2. 删除雨具
        query.prepare(QStringLiteral("DELETE FROM raingear WHERE gear_id = :gid"));
        query.bindValue(":gid", gearId.trimmed());
        
        if (!query.exec()) {
            qWarning() << "[DB] 删除雨具失败:" << query.lastError().text();
            db.rollback();
            return false;
        }
        
        // 提交事务
        if (!db.commit()) {
            qWarning() << "[DB] 提交事务失败";
            db.rollback();
            return false;
        }
        
        qInfo() << "[DB] 删除雨具成功: gear_id=" << gearId;
        return true;
        
    } catch (...) {
        db.rollback();
        qWarning() << "[DB] 删除雨具操作异常，已回滚";
        return false;
    }
}

// ========== 借还操作 ==========
bool DatabaseManager::borrowGear(const QString &userId, const QString &gearId, int stationId, int slotId, double deposit)
{
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return false;
    }

    // 开启事务
    if (!db.transaction()) {
        qWarning() << "[DB] 无法开启事务";
        return false;
    }

    QSqlQuery query(db);
    bool success = true;

    try {
        // 1. 检查用户余额是否充足
        query.prepare(QStringLiteral("SELECT credit FROM users WHERE user_id = :uid LIMIT 1"));
        query.bindValue(":uid", userId);
        if (!query.exec() || !query.next()) {
            qWarning() << "[DB] 用户不存在";
            db.rollback();
            return false;
        }
        double currentBalance = query.value(0).toDouble();
        if (currentBalance < deposit) {
            qWarning() << "[DB] 余额不足，当前余额:" << currentBalance << "需要:" << deposit;
            db.rollback();
            return false;
        }

        // 2. 检查雨具是否可借
        query.prepare(QStringLiteral(
            "SELECT status FROM raingear WHERE gear_id = :gid AND station_id = :sid AND slot_id = :slid LIMIT 1"));
        query.bindValue(":gid", gearId);
        query.bindValue(":sid", stationId);
        query.bindValue(":slid", slotId);
        if (!query.exec() || !query.next()) {
            qWarning() << "[DB] 雨具不存在或位置不匹配";
            db.rollback();
            return false;
        }
        int gearStatus = query.value(0).toInt();
        if (gearStatus != 1) { // 1表示可借
            qWarning() << "[DB] 雨具不可借，状态:" << gearStatus;
            db.rollback();
            return false;
        }

        // 3. 扣除用户余额
        query.prepare(QStringLiteral("UPDATE users SET credit = credit - :deposit WHERE user_id = :uid"));
        query.bindValue(":deposit", deposit);
        query.bindValue(":uid", userId);
        if (!query.exec()) {
            qWarning() << "[DB] 扣除余额失败:" << query.lastError().text();
            db.rollback();
            return false;
        }

        // 4. 更新雨具状态（设为已借出，清空station_id和slot_id）
        query.prepare(QStringLiteral(
            "UPDATE raingear SET status = 2, station_id = NULL, slot_id = NULL WHERE gear_id = :gid"));
        query.bindValue(":gid", gearId);
        if (!query.exec()) {
            qWarning() << "[DB] 更新雨具状态失败:" << query.lastError().text();
            db.rollback();
            return false;
        }

        // 5. 创建借还记录
        query.prepare(QStringLiteral(
            "INSERT INTO record (user_id, gear_id, borrow_time, cost) "
            "VALUES (:uid, :gid, NOW(), 0.00)"));
        query.bindValue(":uid", userId);
        query.bindValue(":gid", gearId);
        if (!query.exec()) {
            qWarning() << "[DB] 创建借还记录失败:" << query.lastError().text();
            db.rollback();
            return false;
        }

        // 提交事务
        if (!db.commit()) {
            qWarning() << "[DB] 提交事务失败";
            db.rollback();
            return false;
        }

        qInfo() << "[DB] 借伞成功: 用户" << userId << "借出雨具" << gearId;
        return true;

    } catch (...) {
        db.rollback();
        qWarning() << "[DB] 借伞操作异常，已回滚";
        return false;
    }
}

std::pair<bool, double> DatabaseManager::returnGear(const QString &userId, const QString &gearId, int stationId, int slotId)
{
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return {false, 0.0};
    }

    // 开启事务
    if (!db.transaction()) {
        qWarning() << "[DB] 无法开启事务";
        return {false, 0.0};
    }

    QSqlQuery query(db);
    double cost = 0.0;

    try {
        // 1. 查找未归还的借还记录
        query.prepare(QStringLiteral(
            "SELECT record_id, borrow_time FROM record "
            "WHERE user_id = :uid AND gear_id = :gid AND return_time IS NULL LIMIT 1"));
        query.bindValue(":uid", userId);
        query.bindValue(":gid", gearId);
        if (!query.exec() || !query.next()) {
            qWarning() << "[DB] 未找到对应的借出记录";
            db.rollback();
            return {false, 0.0};
        }
        qint64 recordId = query.value(0).toLongLong();
        QDateTime borrowTime = query.value(1).toDateTime();

        // 2. 计算费用（24小时内免费，超过24小时按1元/12小时）
        QDateTime now = QDateTime::currentDateTime();
        qint64 hours = borrowTime.secsTo(now) / 3600;
        if (hours > 24) {
            qint64 extraHours = hours - 24;
            cost = (extraHours / 12.0) * 1.0; // 每12小时1元
            if (cost < 0) cost = 0;
        }

        // 3. 检查槽位是否已被占用
        query.prepare(QStringLiteral(
            "SELECT COUNT(*) FROM raingear WHERE station_id = :sid AND slot_id = :slid AND status = 1"));
        query.bindValue(":sid", stationId);
        query.bindValue(":slid", slotId);
        if (!query.exec() || !query.next()) {
            qWarning() << "[DB] 检查槽位失败";
            db.rollback();
            return {false, 0.0};
        }
        if (query.value(0).toInt() > 0) {
            qWarning() << "[DB] 槽位已被占用";
            db.rollback();
            return {false, 0.0};
        }

        // 4. 获取雨具押金（需要根据type_id计算）
        // 注意：借出时雨具的station_id和slot_id为NULL，但type_id仍然存在
        query.prepare(QStringLiteral("SELECT type_id FROM raingear WHERE gear_id = :gid LIMIT 1"));
        query.bindValue(":gid", gearId);
        if (!query.exec() || !query.next()) {
            qWarning() << "[DB] 查询雨具类型失败，gear_id:" << gearId;
            db.rollback();
            return {false, 0.0};
        }
        int typeId = query.value(0).toInt();
        double deposit = 0.0;
        switch (typeId) {
            case 1: deposit = 10.0; break;  // 普通塑料伞
            case 2: deposit = 20.0; break;  // 高质量抗风伞
            case 3: deposit = 15.0; break; // 遮阳伞
            case 4: deposit = 25.0; break; // 雨衣
            default: deposit = 20.0; break;
        }
        qDebug() << "[DB] 雨具类型:" << typeId << "押金:" << deposit;

        // 5. 更新雨具状态（设为可借，设置station_id和slot_id）
        query.prepare(QStringLiteral(
            "UPDATE raingear SET status = 1, station_id = :sid, slot_id = :slid WHERE gear_id = :gid"));
        query.bindValue(":sid", stationId);
        query.bindValue(":slid", slotId);
        query.bindValue(":gid", gearId);
        if (!query.exec()) {
            qWarning() << "[DB] 更新雨具状态失败:" << query.lastError().text();
            db.rollback();
            return {false, 0.0};
        }

        // 6. 退还押金（扣除费用后）
        double refund = deposit - cost;
        if (refund > 0) {
            query.prepare(QStringLiteral("UPDATE users SET credit = credit + :refund WHERE user_id = :uid"));
            query.bindValue(":refund", refund);
            query.bindValue(":uid", userId);
            if (!query.exec()) {
                qWarning() << "[DB] 退还押金失败:" << query.lastError().text();
                db.rollback();
                return {false, 0.0};
            }
        }

        // 7. 更新借还记录
        query.prepare(QStringLiteral(
            "UPDATE record SET return_time = NOW(), cost = :cost WHERE record_id = :rid"));
        query.bindValue(":cost", cost);
        query.bindValue(":rid", recordId);
        if (!query.exec()) {
            qWarning() << "[DB] 更新借还记录失败:" << query.lastError().text();
            db.rollback();
            return {false, 0.0};
        }

        // 提交事务
        if (!db.commit()) {
            qWarning() << "[DB] 提交事务失败";
            db.rollback();
            return {false, 0.0};
        }

        qInfo() << "[DB] 还伞成功: 用户" << userId << "归还雨具" << gearId << "费用:" << cost;
        return {true, cost};

    } catch (...) {
        db.rollback();
        qWarning() << "[DB] 还伞操作异常，已回滚";
        return {false, 0.0};
    }
}

std::optional<DatabaseManager::BorrowRecord> DatabaseManager::fetchUserCurrentBorrow(const QString &userId)
{
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return std::nullopt;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT record_id, user_id, gear_id, borrow_time, return_time, cost "
        "FROM record WHERE user_id = :uid AND return_time IS NULL LIMIT 1"));
    query.bindValue(":uid", userId);
    
    if (!query.exec()) {
        qWarning() << "[DB] 查询借还记录失败:" << query.lastError().text();
        return std::nullopt;
    }

    if (query.next()) {
        BorrowRecord rec;
        rec.recordId = query.value(0).toLongLong();
        rec.userId = query.value(1).toString();
        rec.gearId = query.value(2).toString();
        rec.borrowTime = query.value(3).toString();
        rec.returnTime = query.value(4).toString();
        rec.cost = query.value(5).toDouble();
        return rec;
    }
    
    return std::nullopt;
}

// ========== 管理员功能 ==========
QVector<DatabaseManager::UserRecord> DatabaseManager::fetchAllUsers()
{
    QVector<UserRecord> users;
    
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return users;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT user_id, real_name, credit, role, is_active, password "
        "FROM users ORDER BY user_id"));

    if (!query.exec()) {
        qWarning() << "[DB] 查询用户失败:" << query.lastError().text();
        return users;
    }

    while (query.next()) {
        UserRecord rec;
        rec.userId = query.value(0).toString();
        rec.realName = query.value(1).toString();
        rec.credit = query.value(2).toDouble();
        rec.role = query.value(3).toInt();
        rec.isActive = query.value(4).toBool();
        rec.password = query.value(5).toString();
        users.append(rec);
    }

    return users;
}

bool DatabaseManager::resetUserPassword(const QString &userId, const QString &newPassword)
{
    if (userId.trimmed().isEmpty() || newPassword.isEmpty()) {
        qWarning() << "[DB] 空账号或密码";
        return false;
    }

    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return false;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral("UPDATE users SET password = :pwd WHERE user_id = :uid"));
    query.bindValue(":pwd", newPassword);
    query.bindValue(":uid", userId.trimmed());
    
    if (!query.exec()) {
        qWarning() << "[DB] 重置密码失败:" << query.lastError().text();
        return false;
    }
    
    qInfo() << "[DB] 管理员重置用户" << userId << "的密码";
    return true;
}

bool DatabaseManager::updateGearStatus(const QString &gearId, int newStatus)
{
    if (gearId.trimmed().isEmpty()) {
        qWarning() << "[DB] 雨具ID不能为空";
        return false;
    }
    
    if (newStatus < 1 || newStatus > 3) {
        qWarning() << "[DB] 状态值无效:" << newStatus;
        return false;
    }
    
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) {
        qWarning() << "[DB] 数据库连接失败";
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare(QStringLiteral("UPDATE raingear SET status = :status WHERE gear_id = :gid"));
    query.bindValue(":status", newStatus);
    query.bindValue(":gid", gearId.trimmed());
    
    if (!query.exec()) {
        qWarning() << "[DB] 更新雨具状态失败:" << query.lastError().text();
        return false;
    }
    
    qInfo() << "[DB] 管理员更新雨具" << gearId << "状态为" << newStatus;
    return true;
}


