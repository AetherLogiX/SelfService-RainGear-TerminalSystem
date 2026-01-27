/*
    管理员站点管理服务实现
*/
#include "Admin_StationService.h"
#include "../utils/ConnectionPool.h"

// 获取所有站点的统计信息
QVector<StationStats> Admin_StationService::getStationStats() {
    QSqlDatabase db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return {};
    return stationDao.selectAllWithStats(db);
}

// 获取设备在线率
double Admin_StationService::getOnlineRate() {
    QSqlDatabase db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return 0.0;
    return stationDao.getOnlineRate(db);
}

// 更新站点在线状态
bool Admin_StationService::updateStationStatus(int stationId, bool isOnline) {
    QSqlDatabase db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return false;
    return stationDao.updateStatus(db, stationId, isOnline);
}