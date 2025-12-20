
#include <QList>
#include <QString>
#include <memory>

#include "GlobalEnum.hpp"
#include "RainGear.hpp"
#include "RainGearFactory.h"
#include "RainGear_subclasses.hpp"
#include "StationUtils.h"
#include "Stationlocal.h"

Stationlocal::Stationlocal(Station station, double posX, double posY):station(station),posX(posX),posY(posY){
    inventory.resize(N+1);  // 调整大小为N+1，索引0不使用，索引1-12对应slot_id
}


//判断雨具是否损坏，是否可用
//index范围：1-12（与数据库slot_id一致）
bool Stationlocal::is_gear_available(int index) const{
    if(index<1 || index>N) return false; //索引范围1-12
    if(!inventory[index] || inventory[index]->is_broken()) return false;  
    return true;
}


//库存总数量与可用数量
int Stationlocal::get_inventory_count() const{
    int count = 0;
    for(int i=1;i<=N;++i){
        if(inventory[i]) count++;
    }
    return count; 
}

int Stationlocal::get_available_count() const{
    int count = 0;
    for(int i=1;i<=N;++i){
        if(inventory[i] && !inventory[i]->is_broken()) count++;
    }
    return count;
}

//服务器管理员权限
void Stationlocal::add_gear(int index, std::unique_ptr<RainGear> gear){
    if(index<1||index>N||!gear) return; 
    inventory[index] = std::move(gear);
} //添加雨具到库存

std::unique_ptr<RainGear> Stationlocal::take_gear(int index){
    if(index<1 || index>N || !inventory[index]) return nullptr; 
    return std::move(inventory[index]);
} //从库存中取出雨具

void Stationlocal::mark_unavailable(int index){
    if(index<1 || index>N || !inventory[index]) return; 
    inventory[index]->set_broken(true);
    unavailable_gears.insert(index);
}//标记雨具不可用

void Stationlocal::mark_available(int index){
    if(index<1 || index>N || !inventory[index]) return; 
    inventory[index]->set_broken(false);
    unavailable_gears.remove(index);
} //标记雨具可用

void Stationlocal::set_online(bool online){
    this->online = online;
} //设置站点在线状态

bool Stationlocal::is_online() const{
    return online;
} //获取站点在线状态