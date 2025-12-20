#include "ApiClient.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

ApiClient& ApiClient::instance() {
    static ApiClient instance;
    return instance;
}

ApiClient::ApiClient(QObject* parent) : QObject(parent) {
    m_manager = new QNetworkAccessManager(this);
}

void ApiClient::setServerUrl(const QString& url) {
    m_serverUrl = url;
    if (m_serverUrl.endsWith('/')) {
        m_serverUrl.chop(1);
    }
    qDebug() << "API Server URL set to:" << m_serverUrl;
}

bool ApiClient::checkConnection() {
    auto response = get("/api/health");
    return response.success;
}

// ========== HTTP 请求实现 ==========

ApiClient::ApiResponse ApiClient::get(const QString& endpoint, int timeout) {
    ApiResponse result;
    
    QUrl url(m_serverUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    
    QNetworkReply* reply = m_manager->get(request);
    
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    
    timer.start(timeout);
    loop.exec();
    
    if (timer.isActive()) {
        timer.stop();
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                result.success = obj["success"].toBool();
                result.message = obj["message"].toString();
                if (obj["data"].isObject()) {
                    result.data = obj["data"].toObject();
                } else if (obj["data"].isArray()) {
                    result.dataArray = obj["data"].toArray();
                }
            }
        } else {
            result.message = reply->errorString();
            qWarning() << "API GET Error:" << endpoint << reply->errorString();
        }
    } else {
        result.message = "请求超时";
        qWarning() << "API GET Timeout:" << endpoint;
    }
    
    reply->deleteLater();
    return result;
}

ApiClient::ApiResponse ApiClient::post(const QString& endpoint, const QJsonObject& body, int timeout) {
    ApiResponse result;
    
    QUrl url(m_serverUrl + endpoint);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QByteArray postData = QJsonDocument(body).toJson();
    
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    
    QNetworkReply* reply = m_manager->post(request, postData);
    
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    
    timer.start(timeout);
    loop.exec();
    
    if (timer.isActive()) {
        timer.stop();
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                result.success = obj["success"].toBool();
                result.message = obj["message"].toString();
                if (obj["data"].isObject()) {
                    result.data = obj["data"].toObject();
                } else if (obj["data"].isArray()) {
                    result.dataArray = obj["data"].toArray();
                }
            }
        } else {
            result.message = reply->errorString();
            qWarning() << "API POST Error:" << endpoint << reply->errorString();
        }
    } else {
        result.message = "请求超时";
        qWarning() << "API POST Timeout:" << endpoint;
    }
    
    reply->deleteLater();
    return result;
}

// ========== 用户相关 API ==========

std::optional<ApiClient::UserRecord> ApiClient::verifyUser(const QString& userId, const QString& realName) {
    QJsonObject body;
    body["user_id"] = userId;
    body["real_name"] = realName;
    
    auto response = post("/api/user/verify", body);
    
    if (response.success) {
        UserRecord user;
        user.userId = response.data["user_id"].toString();
        user.realName = response.data["real_name"].toString();
        user.credit = response.data["credit"].toDouble();
        user.role = response.data["role"].toInt();
        user.isActive = response.data["is_active"].toBool();
        return user;
    }
    return std::nullopt;
}

std::optional<ApiClient::UserRecord> ApiClient::login(const QString& userId, const QString& realName, const QString& password) {
    QJsonObject body;
    body["user_id"] = userId;
    body["real_name"] = realName;
    body["password"] = password;
    
    auto response = post("/api/user/login", body);
    
    if (response.success) {
        UserRecord user;
        user.userId = response.data["user_id"].toString();
        user.realName = response.data["real_name"].toString();
        user.credit = response.data["credit"].toDouble();
        user.role = response.data["role"].toInt();
        user.isActive = true;
        return user;
    }
    return std::nullopt;
}

bool ApiClient::activateUser(const QString& userId, const QString& newPassword) {
    QJsonObject body;
    body["user_id"] = userId;
    body["new_password"] = newPassword;
    
    auto response = post("/api/user/activate", body);
    return response.success;
}

bool ApiClient::changePassword(const QString& userId, const QString& oldPassword, const QString& newPassword) {
    QJsonObject body;
    body["user_id"] = userId;
    body["old_password"] = oldPassword;
    body["new_password"] = newPassword;
    
    auto response = post("/api/user/change_password", body);
    return response.success;
}

std::optional<double> ApiClient::getBalance(const QString& userId) {
    auto response = get("/api/user/balance?user_id=" + userId);
    
    if (response.success) {
        return response.data["credit"].toDouble();
    }
    return std::nullopt;
}

// ========== 站点相关 API ==========

QVector<ApiClient::StationRecord> ApiClient::getAllStations() {
    QVector<StationRecord> stations;
    
    auto response = get("/api/stations");
    
    if (response.success) {
        for (const auto& item : response.dataArray) {
            QJsonObject obj = item.toObject();
            StationRecord station;
            station.stationId = obj["station_id"].toInt();
            station.name = obj["name"].toString();
            station.posX = static_cast<float>(obj["pos_x"].toDouble());
            station.posY = static_cast<float>(obj["pos_y"].toDouble());
            station.status = obj["status"].toInt();
            stations.append(station);
        }
    }
    
    return stations;
}

// ========== 雨具相关 API ==========

QVector<ApiClient::GearRecord> ApiClient::getGearsByStation(int stationId) {
    QVector<GearRecord> gears;
    
    auto response = get(QString("/api/station/%1/gears").arg(stationId));
    
    if (response.success) {
        for (const auto& item : response.dataArray) {
            QJsonObject obj = item.toObject();
            GearRecord gear;
            gear.gearId = obj["gear_id"].toString();
            gear.typeId = obj["type_id"].toInt();
            gear.stationId = stationId;
            gear.slotId = obj["slot_id"].toInt();
            gear.status = obj["status"].toInt();
            gears.append(gear);
        }
    }
    
    return gears;
}

std::optional<ApiClient::GearRecord> ApiClient::getGearById(const QString& gearId) {
    auto response = get("/api/gear/" + gearId);
    
    if (response.success) {
        GearRecord gear;
        gear.gearId = response.data["gear_id"].toString();
        gear.typeId = response.data["type_id"].toInt();
        gear.stationId = response.data["station_id"].toInt();
        gear.slotId = response.data["slot_id"].toInt();
        gear.status = response.data["status"].toInt();
        return gear;
    }
    return std::nullopt;
}

// ========== 借还操作 API ==========

std::pair<bool, QString> ApiClient::borrowGear(const QString& userId, const QString& gearId, int stationId, int slotId) {
    QJsonObject body;
    body["user_id"] = userId;
    body["gear_id"] = gearId;
    body["station_id"] = stationId;
    body["slot_id"] = slotId;
    
    auto response = post("/api/borrow", body);
    return {response.success, response.message};
}

std::tuple<bool, double, QString> ApiClient::returnGear(const QString& userId, const QString& gearId, int stationId, int slotId) {
    QJsonObject body;
    body["user_id"] = userId;
    body["gear_id"] = gearId;
    body["station_id"] = stationId;
    body["slot_id"] = slotId;
    
    auto response = post("/api/return", body);
    
    double cost = 0.0;
    if (response.success && response.data.contains("cost")) {
        cost = response.data["cost"].toDouble();
    }
    
    return {response.success, cost, response.message};
}

std::optional<ApiClient::BorrowRecord> ApiClient::getCurrentBorrow(const QString& userId) {
    auto response = get("/api/user/current_borrow?user_id=" + userId);
    
    if (response.success && !response.data.isEmpty()) {
        BorrowRecord record;
        record.recordId = response.data["record_id"].toVariant().toLongLong();
        record.userId = response.data["user_id"].toString();
        record.gearId = response.data["gear_id"].toString();
        record.borrowTime = response.data["borrow_time"].toString();
        record.returnTime = response.data["return_time"].toString();
        record.cost = response.data["cost"].toDouble();
        record.typeId = response.data["type_id"].toInt();
        record.typeName = response.data["type_name"].toString();
        return record;
    }
    return std::nullopt;
}

