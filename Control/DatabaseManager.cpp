#include "DatabaseManager.h"
#include "DBHelper.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

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


