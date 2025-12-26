#pragma once

#include<QSqlDatabase>
#include<QVector>
#include<optional>
#include<memory>

#include<../Model/Stationlocal.hpp>
#include"../Model/GlobalEnum.hpp"

class StationDao{
public:
    //获取所有站点的完整信息
    QVector<std::unique_ptr<Stationlocal>> selectAll(QSqlDatabase& db); 
    //根据站点ID获取站点信息
    std::unique_ptr<Stationlocal> selectById(QSqlDatabase& db, Station station);
};