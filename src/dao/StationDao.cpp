#include"StationDao.h"
#include"GearDao.h"

#include<QSqlQuery>
#include<QSqlError>
#include<QDebug>
#include<QStringList>


// select_all，查出所有站点包含的所有的雨具的完整信息
std::vector<std::unique_ptr<Stationlocal>> StationDao::selectAll(QSqlDatabase& db) {
    std::vector<std::unique_ptr<Stationlocal>> stationList;
    stationList.reserve(20);
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT * FROM station ORDER BY station_id"));

    if (!query.exec()) {
        qCritical() << "查询站点失败:" << query.lastError().text();
        return stationList;
    }

    // 用于查询某个站点的所有雨具的信息，将雨具信息传给Stationlocal类
    GearDao gearDao;
    while (query.next()) {
        // 从station表读基础数据信息
        int idInt=query.value("station_id").toInt();
        Station sid=static_cast<Station>(idInt);
        double x=query.value("pos_x").toDouble();
        double y=query.value("pos_y").toDouble();
        bool isOnline=(query.value("status").toInt()==1);
        QString badSlotsStr =query.value("unavailable_slots").toString();

        // 创建Stationlocal对象
        auto stationObj=std::make_unique<Stationlocal>(sid, x, y);
        stationObj->set_online(isOnline);

        // 解析故障槽位字符串，数据库中是以逗号分隔的字符串如"1,5"
        if (!badSlotsStr.isEmpty()) {
            QStringList slotList = badSlotsStr.split(',', Qt::SkipEmptyParts);
            for (const QString& s : slotList) {
                int slotIndex = s.toInt();
                // 标记该槽位不可用
                stationObj->mark_unavailable(slotIndex);
            }
        }

        // 调用GearDao查出该站点下的所有伞
        auto gears = gearDao.selectByStation(db, sid);
        for (size_t i = 0; i < gears.size(); ++i) {
            auto& gear = gears[i];
            if (gear) {
                int slotIdx = gear->get_slot_id();
                stationObj->add_gear(slotIdx, std::move(gear));
            }
        }

        stationList.push_back(std::move(stationObj));
    }

    return stationList;
}

// select_by_id，查出单个站点包含的所有的雨具的完整信息
std::unique_ptr<Stationlocal> StationDao::selectById(QSqlDatabase& db, Station stationId) {
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT * FROM station WHERE station_id = ?"));
    query.addBindValue(static_cast<int>(stationId));

    if (!query.exec()) {
        qCritical() << "查询站点失败:" << query.lastError().text();
        return nullptr;
    }

    if (query.next()) {
        // 从station表读基础数据信息
        double x = query.value("pos_x").toDouble();
        double y = query.value("pos_y").toDouble();
        bool isOnline = (query.value("status").toInt() == 1);
        QString badSlotsStr = query.value("unavailable_slots").toString();
        // 创建Stationlocal对象
        auto stationObj = std::make_unique<Stationlocal>(stationId, x, y);
        stationObj->set_online(isOnline);

        if (!badSlotsStr.isEmpty()) {
            QStringList slotList = badSlotsStr.split(',', Qt::SkipEmptyParts);
            for (const QString& s : slotList) {
                stationObj->mark_unavailable(s.toInt());
            }
        }

        // 填充雨具
        GearDao gearDao;
        auto gears = gearDao.selectByStation(db, stationId);
        for (size_t i = 0; i < gears.size(); ++i) {
            auto& gear = gears[i];
            if (gear) {
                int slotIdx = gear->get_slot_id();
                stationObj->add_gear(slotIdx, std::move(gear));
            }
        }
        return stationObj;
    }

    return nullptr;
}

// 获取各站点的地图信息（库存数量和在线状态，用于地图显示）
QMap<int, StationMapInfo> StationDao::selectStationMapInfo(QSqlDatabase& db) {
    QMap<int, StationMapInfo> result;
    
    // 先查询所有站点，初始化在线状态和库存数量
    QSqlQuery stationQuery(db);
    stationQuery.prepare(QStringLiteral("SELECT station_id, status FROM station ORDER BY station_id"));
    if (stationQuery.exec()) {
        while (stationQuery.next()) {
            int stationId = stationQuery.value("station_id").toInt();
            bool isOnline = (stationQuery.value("status").toInt() == 1);
            StationMapInfo info;
            info.isOnline = isOnline;
            info.availableCount = 0;  // 初始化为0
            result[stationId] = info;
        }
    }
    
    // 使用聚合查询统计每个站点的可用雨具数量
    QSqlQuery gearQuery(db);
    gearQuery.prepare(QStringLiteral(
        "SELECT station_id, COUNT(*) as available_count "
        "FROM raingear "
        "WHERE station_id IS NOT NULL AND status = 1 "
        "GROUP BY station_id"
    ));
    
    if (!gearQuery.exec()) {
        qCritical() << "查询站点库存失败:" << gearQuery.lastError().text();
        return result;
    }
    
    while (gearQuery.next()) {
        int stationId = gearQuery.value("station_id").toInt();
        int count = gearQuery.value("available_count").toInt();
        if (result.contains(stationId)) {
            result[stationId].availableCount = count;
        }
    }
    
    return result;
}



// 管理员后台Part
// 获取所有站点及其雨具统计
QVector<StationStatsDTO> StationDao::selectAllWithStats(QSqlDatabase& db) {
    QVector<StationStatsDTO> result;
    
    QSqlQuery stationQuery(db);
    stationQuery.prepare(QStringLiteral("SELECT station_id, name, status FROM station ORDER BY station_id"));
    if (!stationQuery.exec()) { return result; }
    
    while (stationQuery.next()) {
        StationStatsDTO stats;
        stats.stationId = stationQuery.value("station_id").toInt();
        stats.name = stationQuery.value("name").toString();
        stats.isOnline = stationQuery.value("status").toInt() == 1;
        stats.totalGears = 0;
        stats.availableCount = 0;
        stats.borrowedCount = 0;
        stats.brokenCount = 0;
        
        // 查询该站点的雨具统计
        QSqlQuery gearQuery(db);
        gearQuery.prepare(QStringLiteral("SELECT status, COUNT(*) as cnt FROM raingear WHERE station_id = ? GROUP BY status"));
        gearQuery.addBindValue(stats.stationId);
        if (gearQuery.exec()) {
            while (gearQuery.next()) {
                int status = gearQuery.value("status").toInt();
                int count = gearQuery.value("cnt").toInt();
                stats.totalGears += count;
                if (status == 1) stats.availableCount = count;
                else if (status == 2) stats.borrowedCount = count;
                else if (status == 3) stats.brokenCount = count;
            }
        } else {
            qWarning() << "查询站点" << stats.stationId << "(" << stats.name << ")的雨具统计失败:" << gearQuery.lastError().text();
        }
        result.append(stats);
    }
    return result;
}

// 获取在线率
double StationDao::getOnlineRate(QSqlDatabase& db) {
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT COUNT(*) as total, SUM(CASE WHEN status = 1 THEN 1 ELSE 0 END) as online FROM station"));
    if (query.exec() && query.next()) {
        int total = query.value("total").toInt();
        int online = query.value("online").toInt();
        if (total > 0) { return (online * 100.0) / total; }
    }
    return 0.0;
}

// 更新站点在线状态
bool StationDao::updateStatus(QSqlDatabase& db, int stationId, bool isOnline) {
    QSqlQuery query(db);
    query.prepare(QStringLiteral("UPDATE station SET status = ? WHERE station_id = ?"));
    query.addBindValue(isOnline ? 1 : 0);
    query.addBindValue(stationId);
    
    if (!query.exec()) {
        qCritical() << "更新站点状态失败:" << query.lastError().text();
        return false;
    }
    
    if (query.numRowsAffected() == 0) {
        qWarning() << "更新站点状态：未找到对应的站点ID" << stationId;
        return false;
    }
    
    return true;
}