#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QMap>
#include <QVector>
#include <memory>

#include "Control/DatabaseManager.h"

class QStackedWidget;
class QWidget;
class QPushButton;
class QLabel;
class QLineEdit;
class QComboBox;
class QTableWidget;
class QTextBrowser;

class AdminMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit AdminMainWindow(QWidget *parent = nullptr);
    ~AdminMainWindow() override = default;

private slots:
    void onRefreshTimer();  // 定时刷新同步

private:
    enum class Page {
        Login,
        Dashboard,
        GearManage,
        UserManage,
        OrderManage
    };

    void setupUi();
    QWidget* createLoginPage();
    QWidget* createDashboardPage();
    QWidget* createGearManagePage();
    QWidget* createUserManagePage();
    QWidget* createOrderManagePage();
    void handleLogout();
    void switchPage(Page page);
    
    // 地图相关
    void loadMapStations(QWidget *mapContainer);
    void onStationClicked(int stationId);
    
    // 数据刷新
    void refreshDashboardData();
    void refreshGearManageData();
    void refreshUserManageData();
    void refreshOrderManageData();
    
    // 管理员登录
    bool performAdminLogin(const QString &userId, const QString &password);
    
    // 天气信息（模拟）
    QString getWeatherInfo() const;

    QStackedWidget *m_stack { nullptr };
    QString m_currentAdminId;
    QString m_currentAdminName;
    
    // 导航栏
    QPushButton *m_navGearManage { nullptr };
    QPushButton *m_navUserManage { nullptr };
    QPushButton *m_navOrderManage { nullptr };
    
    // 管理员信息标签
    QLabel *m_adminLabel { nullptr };
    
    // 登录页面输入框
    QLineEdit *m_loginUserIdInput { nullptr };
    QLineEdit *m_loginPasswordInput { nullptr };
    
    // 定时刷新（实现实时同步）
    QTimer *m_refreshTimer { nullptr };
    
    // 页面组件
    QLabel *m_weatherLabel { nullptr };
    QLabel *m_onlineDevicesLabel { nullptr };
    QLabel *m_borrowedGearsLabel { nullptr };
    QLabel *m_faultCountLabel { nullptr };
    
    // 雨具管理
    QComboBox *m_gearStationCombo { nullptr };
    QComboBox *m_gearSlotCombo { nullptr };
    QTableWidget *m_gearTable { nullptr };
    
    // 用户管理
    QLineEdit *m_userSearchInput { nullptr };
    QTableWidget *m_userTable { nullptr };
    
    // 订单管理
    QTableWidget *m_orderTable { nullptr };
};

