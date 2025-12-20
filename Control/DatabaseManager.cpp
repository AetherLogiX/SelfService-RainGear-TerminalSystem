#include "DatabaseManager.h"
#include "ApiClient.h"

#include <QDebug>

/**
 * 分布式版本的数据库管理器
 * 通过 HTTP API 与 Flask 服务器通信，不再直接访问数据库
 */

bool DatabaseManager::init()
{
    // 检查服务器连接
    bool connected = ApiClient::instance().checkConnection();
    if (connected) {
        qInfo() << "[API] 服务器连接成功:" << ApiClient::instance().serverUrl();
    } else {
        qWarning() << "[API] 服务器连接失败，请确保服务器正在运行";
    }
    return connected;
}

// ========== 用户相关 ==========

std::optional<DatabaseManager::UserRecord> DatabaseManager::fetchUserByIdAndName(const QString &userId, const QString &realName)
{
    if (userId.trimmed().isEmpty() || realName.trimmed().isEmpty()) {
        return std::nullopt;
    }

    auto result = ApiClient::instance().verifyUser(userId.trimmed(), realName.trimmed());
    
    if (result) {
        UserRecord rec;
        rec.userId = result->userId;
        rec.realName = result->realName;
        rec.credit = result->credit;
        rec.role = result->role;
        rec.isActive = result->isActive;
        rec.password = result->password;
        return rec;
    }
    return std::nullopt;
}

std::optional<DatabaseManager::UserRecord> DatabaseManager::fetchUserByIdAndNameAndPassword(const QString &userId, const QString &realName, const QString &plainPassword)
{
    if (userId.trimmed().isEmpty() || realName.trimmed().isEmpty() || plainPassword.isEmpty()) {
        qWarning() << "[API] 空账号/姓名/密码";
        return std::nullopt;
    }

    auto result = ApiClient::instance().login(userId.trimmed(), realName.trimmed(), plainPassword);
    
    if (result) {
        UserRecord rec;
        rec.userId = result->userId;
        rec.realName = result->realName;
        rec.credit = result->credit;
        rec.role = result->role;
        rec.isActive = result->isActive;
        rec.password = plainPassword;
        qInfo() << "[API] 用户登录成功:" << rec.userId;
        return rec;
    }
    
    qWarning() << "[API] 登录失败：密码错误或用户不存在";
    return std::nullopt;
}

bool DatabaseManager::updateUserPassword(const QString &userId, const QString &newPassword)
{
    if (userId.trimmed().isEmpty() || newPassword.isEmpty()) {
        qWarning() << "[API] 空账号或密码";
        return false;
    }

    bool success = ApiClient::instance().activateUser(userId.trimmed(), newPassword);
    
    if (success) {
        qInfo() << "[API] 用户" << userId << "密码已设置，账户已激活";
    } else {
        qWarning() << "[API] 激活账户失败";
    }
    
    return success;
}

bool DatabaseManager::changeUserPassword(const QString &userId, const QString &oldPassword, const QString &newPassword)
{
    if (userId.trimmed().isEmpty() || oldPassword.isEmpty() || newPassword.isEmpty()) {
        qWarning() << "[API] 空账号或密码";
        return false;
    }

    bool success = ApiClient::instance().changePassword(userId.trimmed(), oldPassword, newPassword);
    
    if (success) {
        qInfo() << "[API] 用户" << userId << "密码已修改";
    } else {
        qWarning() << "[API] 修改密码失败：旧密码错误";
    }
    
    return success;
}

bool DatabaseManager::updateUserBalance(const QString &userId, double amount)
{
    Q_UNUSED(userId)
    Q_UNUSED(amount)
    // 注意：当前API不支持直接更新余额，余额变化通过借还操作自动处理
    qWarning() << "[API] updateUserBalance 在分布式模式下不可用，余额通过借还操作自动管理";
    return false;
}

// ========== 站点相关 ==========

QVector<DatabaseManager::StationRecord> DatabaseManager::fetchAllStations()
{
    QVector<StationRecord> stations;
    
    auto apiStations = ApiClient::instance().getAllStations();
    
    for (const auto& s : apiStations) {
        StationRecord rec;
        rec.stationId = s.stationId;
        rec.name = s.name;
        rec.posX = s.posX;
        rec.posY = s.posY;
        rec.status = s.status;
        stations.append(rec);
    }
    
    qDebug() << "[API] 获取站点列表，共" << stations.size() << "个站点";
    return stations;
}

// ========== 雨具相关 ==========

QVector<DatabaseManager::GearRecord> DatabaseManager::fetchGearsByStation(int stationId)
{
    QVector<GearRecord> gears;
    
    auto apiGears = ApiClient::instance().getGearsByStation(stationId);
    
    for (const auto& g : apiGears) {
        GearRecord rec;
        rec.gearId = g.gearId;
        rec.typeId = g.typeId;
        rec.stationId = g.stationId;
        rec.slotId = g.slotId;
        rec.status = g.status;
        gears.append(rec);
    }
    
    qDebug() << "[API] 获取站点" << stationId << "雨具列表，共" << gears.size() << "件";
    return gears;
}

std::optional<DatabaseManager::GearRecord> DatabaseManager::fetchGearById(const QString &gearId)
{
    auto result = ApiClient::instance().getGearById(gearId);
    
    if (result) {
        GearRecord rec;
        rec.gearId = result->gearId;
        rec.typeId = result->typeId;
        rec.stationId = result->stationId;
        rec.slotId = result->slotId;
        rec.status = result->status;
        return rec;
    }
    
    return std::nullopt;
}

// ========== 借还操作 ==========

bool DatabaseManager::borrowGear(const QString &userId, const QString &gearId, int stationId, int slotId, double deposit)
{
    Q_UNUSED(deposit)  // 押金由服务器根据雨具类型自动计算
    
    auto [success, message] = ApiClient::instance().borrowGear(userId, gearId, stationId, slotId);
    
    if (success) {
        qInfo() << "[API] 借伞成功:" << message;
    } else {
        qWarning() << "[API] 借伞失败:" << message;
    }
    
    return success;
}

std::pair<bool, double> DatabaseManager::returnGear(const QString &userId, const QString &gearId, int stationId, int slotId)
{
    auto [success, cost, message] = ApiClient::instance().returnGear(userId, gearId, stationId, slotId);
    
    if (success) {
        qInfo() << "[API] 还伞成功:" << message << "费用:" << cost;
    } else {
        qWarning() << "[API] 还伞失败:" << message;
    }
    
    return {success, cost};
}

std::optional<DatabaseManager::BorrowRecord> DatabaseManager::fetchUserCurrentBorrow(const QString &userId)
{
    auto result = ApiClient::instance().getCurrentBorrow(userId);
    
    if (result) {
        BorrowRecord rec;
        rec.recordId = result->recordId;
        rec.userId = result->userId;
        rec.gearId = result->gearId;
        rec.borrowTime = result->borrowTime;
        rec.returnTime = result->returnTime;
        rec.cost = result->cost;
        return rec;
    }
    
    return std::nullopt;
}
