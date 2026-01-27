#include"UserDao.h"

#include<QSqlQuery>
#include<QSqlError>
#include<QDebug>
#include<QVariant>

// select_by_id
std::optional<User> UserDao::selectById(QSqlDatabase& db, const QString& id){
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT user_id, real_name, password, role, credit, is_active "
    "FROM users WHERE user_id = :uid LIMIT 1")); //查到一个就不再继续往下查了，id是唯一的
    query.bindValue(":uid", id); //绑定参数，避免sql注入
    if(!query.exec()){
        qWarning() << "[UserDao::selectById] Error: " << query.lastError().text();
        return std::nullopt;
    }
    if(query.next()){
        return User(query.value("user_id").toString(), 
                    query.value("real_name").toString(), 
                    query.value("password").toString(), 
                    query.value("role").toInt(), 
                    query.value("credit").toDouble(), 
                    query.value("is_active").toBool());
    }
    return std::nullopt;
}

// select_by_id_and_name
std::optional<User> UserDao::selectByIdAndName(QSqlDatabase& db, const QString& id, const QString& name){
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT user_id, real_name, password, role, credit, is_active "
    "FROM users WHERE user_id = :uid AND real_name = :name LIMIT 1"));
    query.bindValue(":uid",id);
    query.bindValue(":name",name);
    if(!query.exec()){
        qWarning() << "[UserDao::selectByIdAndName] Error: " << query.lastError().text();
        return std::nullopt;
    }
    if(query.next()){
        return User(query.value("user_id").toString(), 
                    query.value("real_name").toString(), 
                    query.value("password").toString(), 
                    query.value("role").toInt(), 
                    query.value("credit").toDouble(), 
                    query.value("is_active").toBool());
    }
    return std::nullopt;
}

// update_password
bool UserDao::updatePassword(QSqlDatabase& db, const QString& id, const QString& name,const QString& newPassword){
    QSqlQuery query(db);
    query.prepare(QStringLiteral("UPDATE users SET password = :newpwd, is_active = 1 WHERE user_id = :uid AND real_name = :name"));
    query.bindValue(":newpwd",newPassword);
    query.bindValue(":uid",id);
    query.bindValue(":name",name);
    if(!query.exec()){
        qWarning() << "[UserDao::updatePassword] Error: " << query.lastError().text();
        return false;
    }
    return true;
}

// update_balance
bool UserDao::updateBalance(QSqlDatabase& db, const QString& id,double amountchange){
    QSqlQuery query(db);
    query.prepare(QStringLiteral("UPDATE users SET credit = credit + :amount WHERE user_id = :uid"));
    query.bindValue(":amount",amountchange);
    query.bindValue(":uid",id);
    if(!query.exec()){
        qWarning() << "[UserDao::updateBalance] Error: " << query.lastError().text();
        return false;
    }
    return true;
}

// select_all
QVector<User> UserDao::selectAll(QSqlDatabase& db){
    QVector<User> users;
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT user_id, real_name, password, role, credit, is_active "
        "FROM users ORDER BY user_id"
    ));
    if(!query.exec()){
        qWarning() << "[UserDao::selectAll] Error: " << query.lastError().text();
        return users;
    }
    while(query.next()){
        users.append(User(query.value("user_id").toString(), query.value("real_name").toString(), query.value("password").toString(), query.value("role").toInt(), query.value("credit").toDouble(), query.value("is_active").toBool()));
    }
    return users;
}
