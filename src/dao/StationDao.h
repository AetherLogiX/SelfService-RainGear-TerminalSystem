#pragma once

#include<QSqlDatabase>
#include<QMap>
#include<vector>
#include<optional>
#include<memory>

#include"../Model/Stationlocal.h"
#include"../Model/GlobalEnum.hpp"

class StationDao{
public:
    //获取所有站点的完整信息
    std::vector<std::unique_ptr<Stationlocal>> selectAll(QSqlDatabase& db); 
    //根据站点ID获取站点信息
    std::unique_ptr<Stationlocal> selectById(QSqlDatabase& db, Station station);
    //获取各站点的可用库存数量（用于地图显示）
    QMap<int, int> selectStationInventoryCounts(QSqlDatabase& db);
};