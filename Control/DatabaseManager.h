#pragma once

#include <optional>
#include <QString>

// 数据库管理器：负责连接MySQL并查询users表
class DatabaseManager {
public:
    struct UserRecord {
        QString userId;      // 学号/工号
        QString realName;    // 真实姓名
        double credit {0.0}; // 账户余额
        int role {0};        // 0:学生, 1:教职工, 9:管理员
        bool isActive {false}; // 是否已激活（首次登录需设置密码）
        QString password;    // 密码（可能为NULL）
    };

    // 初始化数据库连接
    static bool init();

    // 查询指定学号/工号与姓名的用户（用于首次登录识别）
    static std::optional<UserRecord> fetchUserByIdAndName(const QString &userId, const QString &realName);
    
    // 查询账号/姓名/密码（明文密码，不使用哈希）
    static std::optional<UserRecord> fetchUserByIdAndNameAndPassword(const QString &userId, const QString &realName, const QString &plainPassword);
    
    // 更新用户密码（用于首次登录设置密码，同时设置is_active=1）
    static bool updateUserPassword(const QString &userId, const QString &newPassword);
    
    // 修改密码（已激活用户修改密码）
    static bool changeUserPassword(const QString &userId, const QString &oldPassword, const QString &newPassword);

private:
    static QString connName();
    static bool ensureConnectionOpen();
    static QSqlDatabase connection();
};


