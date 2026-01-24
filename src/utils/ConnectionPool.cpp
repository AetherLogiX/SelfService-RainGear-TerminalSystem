#include<QSqlDatabase>
#include<QSqlQuery>
#include<QDebug>
#include<QString>
#include<QSqlError>
#include<QThread>

#include "ConnectionPool.h"

QSqlDatabase ConnectionPool::getThreadLocalConnection(){
    // 根据线程ID生成唯一的连接名，不用改
    QString connectionName=QString("Conn_%1").arg((quint64)QThread::currentThreadId());
    
    if(QSqlDatabase::contains(connectionName)){
        return QSqlDatabase::database(connectionName);
    }else{
        QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL",connectionName);
        db.setHostName("127.0.0.1");
        db.setPort(3306);
        db.setDatabaseName("rainhub_db"); 
        db.setUserName("root"); 
        db.setPassword("root"); 
        
        if(!db.open()){
            qCritical()<<"Failed to connect to database: "<<db.lastError().text();
        }else{
            qInfo()<<"Connected to database: "<<db.databaseName();
            
            QSqlQuery timezoneQuery(db);
            if (!timezoneQuery.exec("SET time_zone = '+8:00'")) {
                 qWarning() << "设置时区失败:" << timezoneQuery.lastError().text();
            } else {
                 qInfo() << "数据库时区已强制设置为: +8:00";
            }
        }
        return db;
    }
}

void ConnectionPool::removeThreadConnection(){
    QString connectionName=QString("Conn_%1").arg((quint64)QThread::currentThreadId());
    if(QSqlDatabase::contains(connectionName)){
        QSqlDatabase::removeDatabase(connectionName);
    }
}