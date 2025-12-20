/*
定义了客户端用来表示借还站点的类 Stationlocal。
posX 和 posY 表示站点的地理位置坐标，inventory是该站点当前库存的雨具列表。
该类提供了添加雨具到库存和从库存中取出雨具的方法，以及查询当前库存数量的方法。
*/

#pragma once

#include <QSet>
#include <QString>
#include <memory>
#include <vector>

#include "GlobalEnum.hpp"
#include "RainGear.hpp"
#include "RainGearFactory.h"
#include "RainGear_subclasses.hpp"
#include "StationUtils.h"

constexpr int N = 12; //以一个站点最多容纳12把雨具为例，索引范围1-12

//站点实体，维护库存与位置信息。
class Stationlocal{
public:
    Stationlocal(Station station, double posX, double posY);

    Station get_station() const { return station; }
    QString get_name() const { return StationUtils::getChineseName(station); }
    double get_posX() const { return posX; }
    double get_posY() const { return posY; }
    
    //判断雨具是否损坏，是否可用
    bool is_gear_available(int index) const;
    

    //库存总数量与可用数量
    int get_inventory_count() const;
    int get_available_count() const;

    //服务器管理员权限
    //index范围：1-12（与数据库slot_id一致）
    void add_gear(int index, std::unique_ptr<RainGear> gear); //添加雨具到库存
    std::unique_ptr<RainGear> take_gear(int index); //从库存中取出雨具
    void mark_unavailable(int index); //标记雨具不可用
    void mark_available(int index); //标记雨具可用
    void set_online(bool online); //设置站点在线状态
    bool is_online() const; //获取站点在线状态
 
private:
    Station station;
    double posX,posY;
    bool online = true; //默认站点是在线状态
    std::vector<std::unique_ptr<RainGear>> inventory;  // 索引0不使用，索引1-12对应slot_id  
    QSet<int> unavailable_gears;    
};
