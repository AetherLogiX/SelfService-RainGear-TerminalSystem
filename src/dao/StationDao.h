#pragma once

#include<QSqlDatabase>
#include<QMap>
#include<QVector>
#include<vector>
#include<optional>
#include<memory>

#include"../Model/Stationlocal.h"
#include"../Model/GlobalEnum.hpp"

// 站点统计信息DTO，管理员后台用
struct StationStatsDTO {
    int stationId;
    QString name;
    bool isOnline;
    int totalGears;
    int availableCount;
    int borrowedCount;
    int brokenCount;
};

class StationDao{
public:
    //获取所有站点的完整信息
    std::vector<std::unique_ptr<Stationlocal>> selectAll(QSqlDatabase& db); 
    //根据站点ID获取站点信息
    std::unique_ptr<Stationlocal> selectById(QSqlDatabase& db, Station station);
    //获取各站点的可用库存数量（用于地图显示）
    QMap<int, int> selectStationInventoryCounts(QSqlDatabase& db);
    
    //管理员Part
    QVector<StationStatsDTO> selectAllWithStats(QSqlDatabase& db); //获取所有站点及其雨具统计
    double getOnlineRate(QSqlDatabase& db); //获取在线率
    bool updateStatus(QSqlDatabase& db, int stationId, bool isOnline); //更新站点在线状态
};