#pragma once

#include<vector>
#include<memory>
#include<QMap>
#include"../dao/StationDao.h"
#include"../model/Stationlocal.h"

class StationService {
public:
    // 获取所有站点信息（完整加载，用于借还页面）
    std::vector<std::unique_ptr<Stationlocal>> getAllStations();
    // 获取单个站点详情
    std::unique_ptr<Stationlocal> getStationDetail(Station stationId);
    // 获取各站点的地图信息（库存数量和在线状态，用于地图显示）
    QMap<int, StationMapInfo> getStationMapInfo();
    
private:
    StationDao stationDao;
};