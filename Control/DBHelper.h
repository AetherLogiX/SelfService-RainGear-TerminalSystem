#pragma once

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QThread>

class DBHelper{
public:
    static QSqlDatabase getThreadLocalConnection() {
        QString connectionName = QString("Conn_%1").arg((quint64)QThread::currentThreadId());
        
        if (QSqlDatabase::contains(connectionName)) {
            return QSqlDatabase::database(connectionName);
        }
        
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName("127.0.0.1");
        db.setPort(3306);
        db.setDatabaseName("rainhub_db");
        db.setUserName("root");
        db.setPassword("root"); 
        
        if (!db.open()) {
            qCritical() << "Thread DB Connect Error:" << db.lastError().text();
        } else {
            qDebug() << "New DB Connection created for thread:" << QThread::currentThreadId();
        }
        return db;
    }
    
    static void removeThreadConnection() {
        QString connectionName = QString("Conn_%1").arg((quint64)QThread::currentThreadId());
        QSqlDatabase::removeDatabase(connectionName);
    }
};