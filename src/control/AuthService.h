/*
  认证服务
  这里实现关于登录并验证用户信息的业务逻辑
*/
#pragma once

#include<QString>
#include<memory>
#include"../dao/UserDao.h"

class AuthService{
public:
    // 定义登录检查的结果状态，给UI层做判断
    enum class LoginStatus{
        SuccessNormal, // 非首次login in，UI跳转输密码
        SuccessFirstTime, // 首次login in，UI跳转设置新密码
        UserNotFound, // 学号不存在
        NameMismatch, // 姓名不匹配
        DatabaseError, // 数据库连接失败
        AdminNotAllowed, // 管理员账号不能在客户端登录
    };
    // 登录检查,先判断账号密码是否合法
    LoginStatus checkLogin(const QString& id, const QString& name);
    // 验证密码
    bool verifyPassword(const QString& id, const QString& password);
    // 激活账户并设置密码
    bool activateUser(const QString& id, const QString& name, const QString& password);
private:
    UserDao userDao;
};
