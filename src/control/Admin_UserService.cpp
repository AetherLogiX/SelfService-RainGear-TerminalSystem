/*
    管理员用户管理服务实现
*/
#include "Admin_UserService.h"
#include "../utils/ConnectionPool.h"

#include <QDebug>

QVector<User> Admin_UserService::getAllUsers(const QString& searchText) {
    QSqlDatabase db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return {};
    QVector<User> allUsers = userDao.selectAll(db);
    if (searchText.isEmpty()) { return allUsers;}
    
    // 按学号或姓名过滤搜索
    QVector<User> filtered;
    for (const auto& user : allUsers) {
        if (user.get_id().contains(searchText, Qt::CaseInsensitive) ||
            user.get_name().contains(searchText, Qt::CaseInsensitive)) {
            filtered.append(user);
        }
    }
    return filtered;
}

bool Admin_UserService::resetUserPassword(const QString& userId, const QString& newPassword) {
    QSqlDatabase db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return false;
    // 先获取用户名
    auto userOpt = userDao.selectById(db, userId);
    if (!userOpt) return false;
    return userDao.updatePassword(db, userId, userOpt->get_name(), newPassword);
}
