#include "Stationlocal.h"
#include<QDebug>

Stationlocal::Stationlocal(Station station, double posX, double posY):station(station),posX(posX),posY(posY){
    inventory.resize(Station_capacity+1);  // 调整大小为Station_capacity+1，索引0不使用，索引1-12对应slot_id
}


//判断某个槽位的雨具是否可用
bool Stationlocal::is_gear_available(int index) const{
    if(index<1 || index>Station_capacity) return false;
    if(!inventory[index] || inventory[index]->is_broken() ||unavailable_gears.contains(index)) return false;  
    return true;
}


//库存总数量与可用数量
int Stationlocal::get_inventory_count() const{
    int count = 0;
    for(int i=1;i<=Station_capacity;++i){
        if(inventory[i]) count++;
    }
    return count; 
}

int Stationlocal::get_available_count() const{
    int count = 0;
    for(int i=1;i<=Station_capacity;++i){
        if(is_gear_available(i)) count++;
    }
    return count;
}

//管理员权限
void Stationlocal::add_gear(int index, std::unique_ptr<RainGear> gear){
    if(index<1||index>Station_capacity||!gear) return; 
    if(gear->is_broken()) unavailable_gears.insert(index);
    inventory[index] = std::move(gear);
} //添加雨具到库存,dao读取数据库后会调用这个函数把伞放进站点

std::unique_ptr<RainGear> Stationlocal::take_gear(int index){
    if(index<1 || index>Station_capacity || !inventory[index]) return nullptr; 
    unavailable_gears.remove(index); //移出不可用槽位名单
    return std::move(inventory[index]);
} //从库存中取出雨具

void Stationlocal::mark_unavailable(int index){
    if(index<1 || index>Station_capacity || !inventory[index]) return; 
    if(inventory[index]) inventory[index]->set_broken(true); //把雨具的状态也设置成broken
    unavailable_gears.insert(index);
}//标记雨具不可用

void Stationlocal::mark_available(int index){
    if(index<1 || index>Station_capacity || !inventory[index]) return; 
    if(inventory[index]) inventory[index]->set_broken(false);
    unavailable_gears.remove(index);
} //标记雨具可用

void Stationlocal::set_online(bool online){
    this->online = online;
} //设置站点在线状态

bool Stationlocal::is_online() const{
    return online;
} //获取站点在线状态