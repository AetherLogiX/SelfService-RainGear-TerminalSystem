/*
    管理员认证服务实现
*/
#include "Admin_AuthService.h"
#include "../utils/ConnectionPool.h"

#include <QDebug>

std::optional<User> Admin_AuthService::adminLogin(const QString& userId, const QString& password) {
    QSqlDatabase db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) {
        qCritical() << "[Admin_AuthService] 数据库连接失败";
        return std::nullopt;
    }
    
    auto userOpt = userDao.selectById(db, userId);
    if (!userOpt) { return std::nullopt;  }
    
    if (userOpt->get_password() != password) {return std::nullopt; } // 密码错误
    if (userOpt->get_role() != 9) {return std::nullopt; } // 不是管理员
    
    return userOpt;
}
