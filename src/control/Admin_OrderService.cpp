/*
    管理员订单管理服务实现
*/
#include "Admin_OrderService.h"
#include "../utils/ConnectionPool.h"

// 获取最近订单
QVector<OrderInfo> Admin_OrderService::getRecentOrders(int limit) {
    QSqlDatabase db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return {};
    return recordDao.selectRecent(db, limit);
}
