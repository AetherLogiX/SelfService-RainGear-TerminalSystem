#pragma once

#include <optional>
#include <QString>
#include <QVector>

class QSqlDatabase;

// 数据库管理器：负责连接MySQL并查询users表、雨具表、借还记录表
class DatabaseManager {
public:
    struct UserRecord {
        QString userId;      // 学号/工号
        QString realName;    // 真实姓名
        double credit {0.0}; // 账户余额
        int role {0};        // 0:学生, 1:教职工, 9:管理员
        bool isActive {false}; // 是否已激活（首次登录需设置密码）
        QString password;    // 密码（可能为NULL）
    };

    struct StationRecord {
        int stationId;
        QString name;
        float posX;
        float posY;
        int status;
    };

    struct GearRecord {
        QString gearId;      // 雨具ID
        int typeId;         // 类型ID (1-4)
        int stationId;      // 站点ID (NULL表示已借出)
        int slotId;         // 槽位ID (1-12)
        int status;         // 状态 (1:可借, 2:已借出, 3:损坏)
    };

    struct BorrowRecord {
        qint64 recordId;
        QString userId;
        QString gearId;
        QString borrowTime;
        QString returnTime;
        double cost;
    };

    // 初始化数据库连接
    static bool init();

    // ========== 用户相关 ==========
    // 查询指定学号/工号与姓名的用户（用于首次登录识别）
    static std::optional<UserRecord> fetchUserByIdAndName(const QString &userId, const QString &realName);
    
    // 查询账号/姓名/密码（明文密码，不使用哈希）
    static std::optional<UserRecord> fetchUserByIdAndNameAndPassword(const QString &userId, const QString &realName, const QString &plainPassword);
    
    // 更新用户密码（用于首次登录设置密码，同时设置is_active=1）
    static bool updateUserPassword(const QString &userId, const QString &newPassword);
    
    // 修改密码（已激活用户修改密码）
    static bool changeUserPassword(const QString &userId, const QString &oldPassword, const QString &newPassword);

    // 更新用户余额
    static bool updateUserBalance(const QString &userId, double amount);

    // ========== 站点相关 ==========
    // 获取所有站点列表
    static QVector<StationRecord> fetchAllStations();

    // ========== 雨具相关 ==========
    // 根据站点ID查询该站点的所有雨具（包括槽位信息）
    static QVector<GearRecord> fetchGearsByStation(int stationId);

    // 根据gear_id查询雨具信息
    static std::optional<GearRecord> fetchGearById(const QString &gearId);

    // ========== 借还操作 ==========
    // 借伞：更新雨具状态、扣除押金、创建借还记录
    // 返回值：成功返回true，失败返回false
    static bool borrowGear(const QString &userId, const QString &gearId, int stationId, int slotId, double deposit);

    // 还伞：更新雨具状态、退还押金、更新借还记录
    // 返回值：成功返回true，失败返回false，返回扣除的费用
    static std::pair<bool, double> returnGear(const QString &userId, const QString &gearId, int stationId, int slotId);

    // 查询用户当前借出的雨具（未归还的记录）
    static std::optional<BorrowRecord> fetchUserCurrentBorrow(const QString &userId);

private:
    static QString connName();
    static bool ensureConnectionOpen();
    static QSqlDatabase connection();
};


