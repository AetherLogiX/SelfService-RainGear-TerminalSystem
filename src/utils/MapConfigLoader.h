#pragma once

#include<QString>
#include<QMap>
#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>

// 站点静态配置信息结构体,从JSON读取
struct StationConfig {
    int stationId;
    QString name;
    double posX;
    double posY;
    QString description;
};

/*
  静态数据（坐标、名称、描述）从JSON读取，速度极快
  动态数据（库存数量）仍从数据库读取，保证实时性
*/
class MapConfigLoader {
public:
    // 加载所有站点的静态配置
    static QMap<int, StationConfig> loadStationConfigs() {
        QMap<int, StationConfig> configs;
        QFile configFile(":/map/map_config.json");
        if (!configFile.open(QIODevice::ReadOnly)) { return configs; }
        QByteArray data = configFile.readAll(); // 读取JSON文件内容，转换为QByteArray
        configFile.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data); // 将QByteArray转换为QJsonDocument
        if (doc.isNull() || !doc.isObject()) { return configs; }
        // 将QJsonDocument->QJsonObject->QJsonArray
        QJsonObject root = doc.object(); 
        QJsonArray stationsArray = root["stations"].toArray(); 
        
        for (const QJsonValue &value : stationsArray) {
            QJsonObject obj = value.toObject(); 
            StationConfig cfg;
            // 将QJsonObject转换为StationConfig，对应转换JSON文件中的字段
            cfg.stationId = obj["station_id"].toInt(); 
            cfg.name = obj["name"].toString(); 
            cfg.posX = obj["pos_x"].toDouble();
            cfg.posY = obj["pos_y"].toDouble(); 
            cfg.description = obj["description"].toString(); 
            configs[cfg.stationId] = cfg; // 将StationConfig添加到QMap中
        }
        
        return configs; 
    }
};

