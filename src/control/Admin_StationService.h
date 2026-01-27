/*
    管理员站点管理服务
*/
#pragma once

#include <QString>
#include <QVector>
#include "../dao/StationDao.h"

// 复用DAO层的DTO
using StationStats = StationStatsDTO;

class Admin_StationService {
public:
    QVector<StationStats> getStationStats(); // 获取所有站点的统计信息
    double getOnlineRate(); // 获取设备在线率
    bool updateStationStatus(int stationId, bool isOnline); // 更新站点在线状态
private:
    StationDao stationDao;
};
