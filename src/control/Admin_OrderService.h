/*
    管理员订单管理服务
*/
#pragma once

#include <QString>
#include <QVector>
#include "../dao/RecordDao.h"

// 复用DAO层的DTO
using OrderInfo = OrderInfoDTO;

class Admin_OrderService {
public:
    QVector<OrderInfo> getRecentOrders(int limit = 50); // 获取最近订单
private:
    RecordDao recordDao;
};
