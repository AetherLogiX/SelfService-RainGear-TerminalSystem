#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <optional>
#include <QVector>

/**
 * CampusRain HTTP API 客户端
 * 用于与 Flask 服务器通信的分布式客户端
 */
class ApiClient : public QObject {
    Q_OBJECT

public:
    // 数据结构定义
    struct UserRecord {
        QString userId;
        QString realName;
        double credit {0.0};
        int role {0};
        bool isActive {false};
        QString password;
    };

    struct StationRecord {
        int stationId;
        QString name;
        float posX;
        float posY;
        int status;
    };

    struct GearRecord {
        QString gearId;
        int typeId;
        int stationId;
        int slotId;
        int status;
    };

    struct BorrowRecord {
        qint64 recordId;
        QString userId;
        QString gearId;
        QString borrowTime;
        QString returnTime;
        double cost;
        int typeId;
        QString typeName;
    };

    // API 响应结构
    struct ApiResponse {
        bool success {false};
        QString message;
        QJsonObject data;
        QJsonArray dataArray;
    };

    // 单例模式
    static ApiClient& instance();

    // 设置服务器地址
    void setServerUrl(const QString& url);
    QString serverUrl() const { return m_serverUrl; }

    // 检查服务器连接
    bool checkConnection();

    // ========== 用户相关 API ==========
    std::optional<UserRecord> verifyUser(const QString& userId, const QString& realName);
    std::optional<UserRecord> login(const QString& userId, const QString& realName, const QString& password);
    bool activateUser(const QString& userId, const QString& newPassword);
    bool changePassword(const QString& userId, const QString& oldPassword, const QString& newPassword);
    std::optional<double> getBalance(const QString& userId);

    // ========== 站点相关 API ==========
    QVector<StationRecord> getAllStations();
    
    // ========== 雨具相关 API ==========
    QVector<GearRecord> getGearsByStation(int stationId);
    std::optional<GearRecord> getGearById(const QString& gearId);

    // ========== 借还操作 API ==========
    std::pair<bool, QString> borrowGear(const QString& userId, const QString& gearId, int stationId, int slotId);
    std::tuple<bool, double, QString> returnGear(const QString& userId, const QString& gearId, int stationId, int slotId);
    std::optional<BorrowRecord> getCurrentBorrow(const QString& userId);

private:
    explicit ApiClient(QObject* parent = nullptr);
    ~ApiClient() = default;
    
    // 禁用拷贝
    ApiClient(const ApiClient&) = delete;
    ApiClient& operator=(const ApiClient&) = delete;

    // HTTP 请求方法
    ApiResponse get(const QString& endpoint, int timeout = 5000);
    ApiResponse post(const QString& endpoint, const QJsonObject& body, int timeout = 5000);

    QString m_serverUrl {"http://127.0.0.1:5000"};
    QNetworkAccessManager* m_manager {nullptr};
};

