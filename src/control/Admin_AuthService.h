/*
    管理员认证服务
*/
#pragma once

#include <QString>
#include <optional>

#include "../dao/UserDao.h"
#include "../model/User.h"

class Admin_AuthService {
public:
    // 管理员登录验证
    std::optional<User> adminLogin(const QString& userId, const QString& password);
private:
    UserDao userDao;
};
