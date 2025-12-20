/*
定义共同抽象基类雨具类RainGear 
*/
#pragma once
#include <QString>
#include "GlobalEnum.hpp"

// 抽象基类，描述雨具的共有属性与行为
class RainGear {
public:
    RainGear(QString id, GearType type):id(std::move(id)),type(type){}
    virtual ~RainGear()=default;

    QString get_id() const { return id; }
    GearType get_type() const { return type; }

    //纯虚接口：不同品类押金与图标不同。
    virtual double get_deposit() const = 0; //获取押金金额
    virtual QString get_iconpath() const = 0; //获取图标资源路径

    // 损坏状态访问
    bool is_broken() const { return broken_flag; }
    void set_broken(bool broken) { broken_flag = broken; }

private:
    QString id; //加前缀str便于区分，每个伞的id都必须是唯一的
    GearType type; //雨具类型
    bool broken_flag = false; //是否损坏，默认为false
};