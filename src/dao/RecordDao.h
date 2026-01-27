#pragma once
#include<QSqlDatabase>
#include<QString>
#include<QVector>
#include<optional>

#include"../Model/BorrowRecord.h"

// 订单信息DTO,管理员后台用
struct OrderInfoDTO {
    qint64 recordId;
    QString userId;
    QString gearId;
    QString borrowTime;
    QString returnTime;
    double cost;
};

class RecordDao {
public:
    // add借出记录
    bool addBorrowRecord(QSqlDatabase& db, const QString& userId, const QString& gearId);
    // 查找未归还记录,这里需要返回 BorrowRecord 对象给 Service 层用来算钱
    std::optional<BorrowRecord> selectUnfinishedByUserId(QSqlDatabase& db, const QString& userId);
    // 结单,更新归还时间与费用
    bool updateReturnInfo(QSqlDatabase& db, qint64 recordId, const QDateTime& returnTime, double cost);
    
    // 管理员后台Part
    QVector<OrderInfoDTO> selectRecent(QSqlDatabase& db, int limit = 50); // 获取最近订单
};