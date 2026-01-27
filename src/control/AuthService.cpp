#include"AuthService.h"
#include"../utils/ConnectionPool.h"
#include<QDebug>

AuthService::LoginStatus AuthService::checkLogin(const QString& id, const QString& name){
    QSqlDatabase db=ConnectionPool::getThreadLocalConnection();
    if(!db.isOpen()){
        qCritical() << "数据库连接失败";
        return LoginStatus::DatabaseError; 
    }
    auto user=userDao.selectById(db,id);
    if(!user){
        return LoginStatus::UserNotFound; // 学号不存在
    }else if(user->get_name()!=name){
        return LoginStatus::NameMismatch; // 姓名不匹配
    }else if(user->get_role()==9){
        return LoginStatus::AdminNotAllowed; // 管理员账号不能在客户端登录
    }else if(user->get_is_active()==0){
        return LoginStatus::SuccessFirstTime; // 首次login in，UI跳转设置新密码
    }
    return LoginStatus::SuccessNormal; // 非首次login in，UI跳转输密码
}

bool AuthService::verifyPassword(const QString& id, const QString& password){
    QSqlDatabase db=ConnectionPool::getThreadLocalConnection();
    if(!db.isOpen()){
        qCritical() << "数据库连接失败";
        return false;
    }
    auto user=userDao.selectById(db,id);
    if(!user){
        return false;
    }
    return user->get_password()==password;
}

bool AuthService::activateUser(const QString& id, const QString& name, const QString& password){
    QSqlDatabase db=ConnectionPool::getThreadLocalConnection();
    if(!db.isOpen()){
        qCritical() << "数据库连接失败";
        return false;
    }
    if(!db.transaction()){
        qCritical() << "数据库事务开启失败";
        return false;
    }
    bool success=userDao.updatePassword(db,id,name,password);
    if(!success){
        db.rollback();
        return false;
    }
    return db.commit();
}