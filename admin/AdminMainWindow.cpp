#include "AdminMainWindow.h"

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDialog>
#include <QDialogButtonBox>
#include <QBrush>
#include <QColor>
#include <QAbstractItemView>
#include <QSqlQuery>
#include "Control/DBHelper.h"

AdminMainWindow::AdminMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    switchPage(Page::Login);
    setWindowTitle("RainHub 管理员后台");
    resize(900, 700);  // 与客户端相同的尺寸
    
    // 创建定时刷新器（每3秒刷新一次，实现实时同步）
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &AdminMainWindow::onRefreshTimer);
}

void AdminMainWindow::setupUi()
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(createLoginPage());         // 0 Login
    m_stack->addWidget(createDashboardPage());     // 1 Dashboard
    m_stack->addWidget(createGearManagePage());    // 2 GearManage
    m_stack->addWidget(createUserManagePage());    // 3 UserManage
    m_stack->addWidget(createOrderManagePage());   // 4 OrderManage

    layout->addWidget(m_stack);
    setCentralWidget(central);
}

QWidget* AdminMainWindow::createLoginPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    auto *title = new QLabel(tr("管理员后台登录"), page);
    title->setStyleSheet("font-size:28px; font-weight:700; color: #2c3e50;");
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(tr("请输入管理员账号和密码"), page);
    subtitle->setStyleSheet("font-size:16px; color: #7f8c8d;");
    subtitle->setAlignment(Qt::AlignCenter);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setVerticalSpacing(16);
    
    m_loginUserIdInput = new QLineEdit(page);
    m_loginPasswordInput = new QLineEdit(page);
    m_loginUserIdInput->setPlaceholderText(tr("请输入管理员账号"));
    m_loginPasswordInput->setPlaceholderText(tr("请输入密码"));
    m_loginPasswordInput->setEchoMode(QLineEdit::Password);
    m_loginUserIdInput->setFixedWidth(300);
    m_loginPasswordInput->setFixedWidth(300);
    
    form->addRow(tr("管理员账号："), m_loginUserIdInput);
    form->addRow(tr("密码："), m_loginPasswordInput);

    auto *btnLogin = new QPushButton(tr("登录"), page);
    btnLogin->setFixedWidth(160);
    btnLogin->setStyleSheet("font-size:16px; padding:10px; background-color: #3498db; color: white;");
    connect(btnLogin, &QPushButton::clicked, this, [this] {
        const QString userId = m_loginUserIdInput->text().trimmed();
        const QString password = m_loginPasswordInput->text();

        if (userId.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("请输入管理员账号和密码"));
            return;
        }

        if (!DatabaseManager::init()) {
            QMessageBox::critical(this, tr("数据库错误"), tr("无法连接到数据库，请检查服务是否已启动。"));
            return;
        }

        if (performAdminLogin(userId, password)) {
            m_loginPasswordInput->clear();
            switchPage(Page::Dashboard);
            m_refreshTimer->start(3000); // 开始定时刷新（3秒一次）
        }
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(subtitle, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addLayout(form);
    layout->addWidget(btnLogin, 0, Qt::AlignCenter);
    return page;
}

bool AdminMainWindow::performAdminLogin(const QString &userId, const QString &password)
{
    // 查询管理员账号（role=9）
    auto record = DatabaseManager::fetchUserByIdAndNameAndPassword(userId, userId, password);
    if (!record) {
        QMessageBox::warning(this, tr("登录失败"), tr("账号或密码错误"));
        return false;
    }
    
    if (record->role != 9) {
        QMessageBox::warning(this, tr("权限不足"), tr("该账号不是管理员账号"));
        return false;
    }
    
    m_currentAdminId = record->userId;
    m_currentAdminName = record->realName;
    
    // 更新管理员信息标签
    if (m_adminLabel) {
        m_adminLabel->setText(tr("管理员：%1").arg(m_currentAdminId));
    }
    
    QMessageBox::information(this, tr("登录成功"), tr("欢迎，%1").arg(m_currentAdminName));
    return true;
}

QWidget* AdminMainWindow::createDashboardPage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航栏
    auto *sidebar = new QWidget(page);
    sidebar->setMinimumWidth(200);
    sidebar->setMaximumWidth(200);
    sidebar->setStyleSheet(
        "QWidget { background-color: #34495e; }"
        "QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; }"
        "QPushButton:hover { background-color: #2c3e50; }"
    );
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    
    // 导航按钮
    auto *btnDashboard = new QPushButton(tr("📊 首页概览"), sidebar);
    btnDashboard->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: #2c3e50; }");
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    
    auto *btnGear = new QPushButton(tr("☂️ 雨具管理"), sidebar);
    connect(btnGear, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    
    auto *btnUser = new QPushButton(tr("👤 用户管理"), sidebar);
    connect(btnUser, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    
    auto *btnOrder = new QPushButton(tr("📋 订单/流水"), sidebar);
    connect(btnOrder, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });
    
    sidebarLayout->addWidget(btnDashboard);
    sidebarLayout->addWidget(btnGear);
    sidebarLayout->addWidget(btnUser);
    sidebarLayout->addWidget(btnOrder);
    sidebarLayout->addStretch();
    
    // 右侧主内容区
    auto *contentArea = new QWidget(page);
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(16);

    // 顶部栏：标题、天气、管理员信息
    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel(tr("管理员后台"), contentArea);
    title->setStyleSheet("font-size:24px; font-weight:700; color: #2c3e50;");
    
    m_weatherLabel = new QLabel(getWeatherInfo(), contentArea);
    m_weatherLabel->setStyleSheet("font-size:14px; color: #7f8c8d; padding: 8px 16px; background-color: #ecf0f1; border-radius: 4px;");
    
    m_adminLabel = new QLabel(tr("管理员："), contentArea);
    m_adminLabel->setStyleSheet("font-size:14px; color: #e74c3c; font-weight:600;");
    
    auto *btnLogout = new QPushButton(tr("退出登录"), contentArea);
    btnLogout->setFixedWidth(100);
    btnLogout->setStyleSheet("font-size:12px; padding:6px 12px; background-color: #e74c3c; color: white; border-radius: 4px;");
    connect(btnLogout, &QPushButton::clicked, this, &AdminMainWindow::handleLogout);
    
    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(m_weatherLabel);
    topBar->addSpacing(10);
    topBar->addWidget(m_adminLabel);
    topBar->addSpacing(10);
    topBar->addWidget(btnLogout);
    contentLayout->addLayout(topBar);

    // 统计信息栏
    auto *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(12);
    
    m_onlineDevicesLabel = new QLabel(tr("设备在线: 计算中..."), contentArea);
    m_borrowedGearsLabel = new QLabel(tr("雨具借出: 计算中..."), contentArea);
    m_faultCountLabel = new QLabel(tr("待处理故障: 计算中..."), contentArea);
    
    QString statsStyle = "font-size:14px; padding: 12px 20px; background-color: #ecf0f1; border-radius: 6px; font-weight:600;";
    m_onlineDevicesLabel->setStyleSheet(statsStyle);
    m_borrowedGearsLabel->setStyleSheet(statsStyle);
    m_faultCountLabel->setStyleSheet(statsStyle);
    
    statsLayout->addWidget(m_onlineDevicesLabel);
    statsLayout->addWidget(m_borrowedGearsLabel);
    statsLayout->addWidget(m_faultCountLabel);
    statsLayout->addStretch();
    contentLayout->addLayout(statsLayout);

    // 站点概览表格（替代地图）
    auto *tableTitle = new QLabel(tr("📍 站点雨具概览"), contentArea);
    tableTitle->setStyleSheet("font-size:16px; font-weight:600; color: #2c3e50; margin-top: 10px;");
    contentLayout->addWidget(tableTitle);
    
    m_stationTable = new QTableWidget(contentArea);
    m_stationTable->setColumnCount(5);
    m_stationTable->setHorizontalHeaderLabels({
        tr("站点名称"), tr("总雨具数"), tr("可借数量"), tr("已借出"), tr("故障数")
    });
    m_stationTable->horizontalHeader()->setStretchLastSection(true);
    m_stationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_stationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_stationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_stationTable->setStyleSheet("QTableWidget { background-color: white; border: 1px solid #bdc3c7; } QHeaderView::section { background-color: #34495e; color: white; padding: 8px; font-weight: 600; }");
    contentLayout->addWidget(m_stationTable, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);
    
    // 初始化数据
    refreshDashboardData();
    
    return page;
}

QString AdminMainWindow::getWeatherInfo() const
{
    // 模拟天气信息（实际可以接入天气API）
    QDateTime now = QDateTime::currentDateTime();
    int hour = now.time().hour();
    QString weather;
    if (hour >= 6 && hour < 12) {
        weather = tr("天气: 小雨 18°C");
    } else if (hour >= 12 && hour < 18) {
        weather = tr("天气: 多云 22°C");
    } else {
        weather = tr("天气: 小雨 16°C");
    }
    return weather;
}

void AdminMainWindow::refreshDashboardData()
{
    if (!DatabaseManager::init()) return;
    
    // 更新天气
    if (m_weatherLabel) {
        m_weatherLabel->setText(getWeatherInfo());
    }
    
    // 获取所有雨具（包括已借出的）
    auto allGears = DatabaseManager::fetchAllGears();
    auto stations = DatabaseManager::fetchAllStations();
    
    // 统计总数据
    int totalBorrowed = 0;
    int totalFault = 0;
    for (const auto &gear : allGears) {
        if (gear.status == 2) totalBorrowed++;
        else if (gear.status == 3) totalFault++;
    }
    
    // 计算设备在线率
    int onlineCount = 0;
    for (const auto &s : stations) {
        if (s.status == 1) onlineCount++;
    }
    double onlineRate = stations.isEmpty() ? 0 : (onlineCount * 100.0 / stations.size());
    if (m_onlineDevicesLabel) {
        m_onlineDevicesLabel->setText(tr("设备在线: %1%").arg(QString::number(onlineRate, 'f', 0)));
    }
    
    if (m_borrowedGearsLabel) {
        m_borrowedGearsLabel->setText(tr("雨具借出: %1把").arg(totalBorrowed));
    }
    
    if (m_faultCountLabel) {
        m_faultCountLabel->setText(tr("待处理故障: %1").arg(totalFault));
    }
    
    // 刷新站点表格
    if (m_stationTable) {
        m_stationTable->setRowCount(0);
        
        for (const auto &station : stations) {
            // 根据gear_id前缀统计该站点的雨具（如G002_xxx属于站点2）
            QString prefix = QString("G%1_").arg(station.stationId, 3, 10, QChar('0'));
            int total = 0, available = 0, borrowed = 0, fault = 0;
            
            for (const auto &gear : allGears) {
                if (gear.gearId.startsWith(prefix)) {
                    total++;
                    if (gear.status == 1) available++;
                    else if (gear.status == 2) borrowed++;
                    else if (gear.status == 3) fault++;
                }
            }
            
            int row = m_stationTable->rowCount();
            m_stationTable->insertRow(row);
            
            m_stationTable->setItem(row, 0, new QTableWidgetItem(station.name));
            m_stationTable->setItem(row, 1, new QTableWidgetItem(QString::number(total)));
            
            auto *availableItem = new QTableWidgetItem(QString::number(available));
            availableItem->setForeground(QBrush(QColor("#2ecc71"))); // 绿色
            m_stationTable->setItem(row, 2, availableItem);
            
            auto *borrowedItem = new QTableWidgetItem(QString::number(borrowed));
            borrowedItem->setForeground(QBrush(QColor("#3498db"))); // 蓝色
            m_stationTable->setItem(row, 3, borrowedItem);
            
            auto *faultItem = new QTableWidgetItem(QString::number(fault));
            if (fault > 0) {
                faultItem->setForeground(QBrush(QColor("#e74c3c"))); // 红色
            }
            m_stationTable->setItem(row, 4, faultItem);
        }
    }
}

void AdminMainWindow::onRefreshTimer()
{
    // 定时刷新各个页面的数据（实现实时同步）
    int currentPage = m_stack->currentIndex();
    if (currentPage == 1) { // Dashboard
        refreshDashboardData();
    } else if (currentPage == 2) { // GearManage
        refreshGearManageData();
    } else if (currentPage == 3) { // UserManage
        refreshUserManageData();
    } else if (currentPage == 4) { // OrderManage
        refreshOrderManageData();
    }
}

void AdminMainWindow::switchPage(Page page)
{
    m_stack->setCurrentIndex(static_cast<int>(page));
    
    // 更新导航栏按钮样式
    QString normalStyle = "QPushButton { background-color: transparent; }";
    QString selectedStyle = "QPushButton { background-color: #2c3e50; }";
    
    if (m_navGearManage) m_navGearManage->setStyleSheet(normalStyle);
    if (m_navUserManage) m_navUserManage->setStyleSheet(normalStyle);
    if (m_navOrderManage) m_navOrderManage->setStyleSheet(normalStyle);
    
    switch (page) {
        case Page::GearManage:
            if (m_navGearManage) m_navGearManage->setStyleSheet(selectedStyle);
            refreshGearManageData();
            break;
        case Page::UserManage:
            if (m_navUserManage) m_navUserManage->setStyleSheet(selectedStyle);
            refreshUserManageData();
            break;
        case Page::OrderManage:
            if (m_navOrderManage) m_navOrderManage->setStyleSheet(selectedStyle);
            refreshOrderManageData();
            break;
        case Page::Dashboard:
            refreshDashboardData();
            break;
        default:
            break;
    }
}

void AdminMainWindow::handleLogout()
{
    m_refreshTimer->stop();
    m_currentAdminId.clear();
    m_currentAdminName.clear();
    
    // 清空登录输入框
    if (m_loginUserIdInput) m_loginUserIdInput->clear();
    if (m_loginPasswordInput) m_loginPasswordInput->clear();
    
    switchPage(Page::Login);
}

QWidget* AdminMainWindow::createGearManagePage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航栏
    auto *sidebar = new QWidget(page);
    sidebar->setMinimumWidth(200);
    sidebar->setMaximumWidth(200);
    sidebar->setStyleSheet(
        "QWidget { background-color: #34495e; }"
        "QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; }"
        "QPushButton:hover { background-color: #2c3e50; }"
    );
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    
    auto *btnDashboard = new QPushButton(tr("📊 首页概览"), sidebar);
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    
    auto *btnGear = new QPushButton(tr("☂️ 雨具管理"), sidebar);
    btnGear->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: #2c3e50; }");
    connect(btnGear, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    
    auto *btnUser = new QPushButton(tr("👤 用户管理"), sidebar);
    connect(btnUser, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    
    auto *btnOrder = new QPushButton(tr("📋 订单/流水"), sidebar);
    connect(btnOrder, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });
    
    sidebarLayout->addWidget(btnDashboard);
    sidebarLayout->addWidget(btnGear);
    sidebarLayout->addWidget(btnUser);
    sidebarLayout->addWidget(btnOrder);
    sidebarLayout->addStretch();

    // 右侧内容区
    auto *contentArea = new QWidget(page);
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(16);

    auto *title = new QLabel(tr("雨具管理"), contentArea);
    title->setStyleSheet("font-size:20px; font-weight:700; color: #2c3e50;");

    // 筛选区域
    auto *filterLayout = new QHBoxLayout();
    auto *stationLabel = new QLabel(tr("选择站点："), contentArea);
    m_gearStationCombo = new QComboBox(contentArea);
    m_gearStationCombo->addItem(tr("-- 全部站点 --"), 0);
    if (DatabaseManager::init()) {
        auto stations = DatabaseManager::fetchAllStations();
        for (const auto &station : stations) {
            m_gearStationCombo->addItem(station.name, station.stationId);
        }
    }
    m_gearStationCombo->setFixedWidth(200);
    
    auto *slotLabel = new QLabel(tr("选择槽位："), contentArea);
    m_gearSlotCombo = new QComboBox(contentArea);
    m_gearSlotCombo->addItem(tr("-- 全部槽位 --"), 0);
    for (int i = 1; i <= 12; ++i) {
        m_gearSlotCombo->addItem(QStringLiteral("#%1").arg(i), i);
    }
    m_gearSlotCombo->setFixedWidth(150);
    
    auto *btnRefresh = new QPushButton(tr("刷新"), contentArea);
    btnRefresh->setFixedWidth(100);
    connect(btnRefresh, &QPushButton::clicked, this, [this] { refreshGearManageData(); });
    connect(m_gearStationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, [this] { refreshGearManageData(); });
    
    filterLayout->addWidget(stationLabel);
    filterLayout->addWidget(m_gearStationCombo);
    filterLayout->addWidget(slotLabel);
    filterLayout->addWidget(m_gearSlotCombo);
    filterLayout->addWidget(btnRefresh);
    filterLayout->addStretch();

    // 雨具表格
    m_gearTable = new QTableWidget(contentArea);
    m_gearTable->setColumnCount(6);
    m_gearTable->setHorizontalHeaderLabels({
        tr("雨具ID"), tr("类型"), tr("站点"), tr("槽位"), tr("状态"), tr("操作")
    });
    m_gearTable->horizontalHeader()->setStretchLastSection(true);
    m_gearTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_gearTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    contentLayout->addWidget(title);
    contentLayout->addLayout(filterLayout);
    contentLayout->addWidget(m_gearTable, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    return page;
}

void AdminMainWindow::refreshGearManageData()
{
    if (!m_gearTable || !DatabaseManager::init()) return;
    
    m_gearTable->setRowCount(0);
    
    int selectedStationId = m_gearStationCombo ? m_gearStationCombo->currentData().toInt() : 0;
    int selectedSlotId = m_gearSlotCombo ? m_gearSlotCombo->currentData().toInt() : 0;
    
    // 查询所有雨具（包括已借出的）
    QVector<DatabaseManager::GearRecord> allGears = DatabaseManager::fetchAllGears();
    
    // 按站点筛选（如果选择了特定站点）
    if (selectedStationId > 0) {
        QVector<DatabaseManager::GearRecord> filtered;
        for (const auto &gear : allGears) {
            // 检查gear_id前缀是否匹配站点（如G002_xxx属于站点2）
            QString prefix = QString("G%1_").arg(selectedStationId, 3, 10, QChar('0'));
            if (gear.gearId.startsWith(prefix)) {
                filtered.append(gear);
            }
        }
        allGears = filtered;
    }
    
    auto stations = DatabaseManager::fetchAllStations();
    QMap<int, QString> stationNames;
    for (const auto &s : stations) {
        stationNames[s.stationId] = s.name;
    }
    
    QStringList typeNames = {tr("未知"), tr("普通塑料伞"), tr("高质量抗风伞"), tr("专用遮阳伞"), tr("雨衣")};
    QStringList statusNames = {tr("未知"), tr("可借"), tr("已借出"), tr("故障")};
    
    for (const auto &gear : allGears) {
        if (selectedSlotId > 0 && gear.slotId != selectedSlotId) continue;
        
        int row = m_gearTable->rowCount();
        m_gearTable->insertRow(row);
        
        m_gearTable->setItem(row, 0, new QTableWidgetItem(gear.gearId));
        m_gearTable->setItem(row, 1, new QTableWidgetItem(
            gear.typeId >= 1 && gear.typeId <= 4 ? typeNames[gear.typeId] : tr("未知")));
        
        // 站点显示：已借出的雨具显示"已借出"
        QString stationDisplay;
        if (gear.status == 2) {
            stationDisplay = tr("已借出");
        } else {
            stationDisplay = stationNames.value(gear.stationId, tr("未知"));
        }
        m_gearTable->setItem(row, 2, new QTableWidgetItem(stationDisplay));
        
        // 槽位显示：已借出的雨具显示"-"
        QString slotDisplay;
        if (gear.status == 2) {
            slotDisplay = tr("-");
        } else {
            slotDisplay = gear.slotId > 0 ? QStringLiteral("#%1").arg(gear.slotId) : tr("无");
        }
        m_gearTable->setItem(row, 3, new QTableWidgetItem(slotDisplay));
        
        auto *statusItem = new QTableWidgetItem(
            gear.status >= 1 && gear.status <= 3 ? statusNames[gear.status] : tr("未知"));
        if (gear.status == 1) {
            statusItem->setForeground(QBrush(QColor("#2ecc71"))); // 绿色
        } else if (gear.status == 2) {
            statusItem->setForeground(QBrush(QColor("#95a5a6"))); // 灰色
        } else if (gear.status == 3) {
            statusItem->setForeground(QBrush(QColor("#e74c3c"))); // 红色
        }
        m_gearTable->setItem(row, 4, statusItem);
        
        // 操作按钮
        auto *btnModify = new QPushButton(tr("修改状态"));
        btnModify->setStyleSheet("font-size:12px; padding:4px 8px;");
        connect(btnModify, &QPushButton::clicked, this, [this, gear]() {
            // 修改状态对话框
            QDialog dialog(this);
            dialog.setWindowTitle(tr("修改雨具状态"));
            auto *layout = new QVBoxLayout(&dialog);
            
            auto *label = new QLabel(tr("雨具ID: %1\n当前状态: %2")
                .arg(gear.gearId)
                .arg(gear.status == 1 ? tr("可借") : (gear.status == 2 ? tr("已借出") : tr("故障"))));
            layout->addWidget(label);
            
            auto *combo = new QComboBox(&dialog);
            combo->addItem(tr("可借"), 1);
            combo->addItem(tr("已借出"), 2);
            combo->addItem(tr("故障"), 3);
            combo->setCurrentIndex(gear.status - 1);
            layout->addWidget(combo);
            
            auto *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
            connect(btnBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(btnBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
            layout->addWidget(btnBox);
            
            if (dialog.exec() == QDialog::Accepted) {
                int newStatus = combo->currentData().toInt();
                // 使用DatabaseManager更新状态
                if (DatabaseManager::updateGearStatus(gear.gearId, newStatus)) {
                    QMessageBox::information(this, tr("成功"), tr("雨具状态已更新"));
                    refreshGearManageData();
                } else {
                    QMessageBox::critical(this, tr("失败"), tr("更新失败，请重试"));
                }
            }
        });
        
        m_gearTable->setCellWidget(row, 5, btnModify);
    }
    
    m_gearTable->resizeColumnsToContents();
}

QWidget* AdminMainWindow::createUserManagePage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航栏
    auto *sidebar = new QWidget(page);
    sidebar->setMinimumWidth(200);
    sidebar->setMaximumWidth(200);
    sidebar->setStyleSheet(
        "QWidget { background-color: #34495e; }"
        "QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; }"
        "QPushButton:hover { background-color: #2c3e50; }"
    );
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    
    auto *btnDashboard = new QPushButton(tr("📊 首页概览"), sidebar);
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    
    auto *btnGear = new QPushButton(tr("☂️ 雨具管理"), sidebar);
    connect(btnGear, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    
    auto *btnUser = new QPushButton(tr("👤 用户管理"), sidebar);
    btnUser->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: #2c3e50; }");
    connect(btnUser, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    
    auto *btnOrder = new QPushButton(tr("📋 订单/流水"), sidebar);
    connect(btnOrder, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });
    
    sidebarLayout->addWidget(btnDashboard);
    sidebarLayout->addWidget(btnGear);
    sidebarLayout->addWidget(btnUser);
    sidebarLayout->addWidget(btnOrder);
    sidebarLayout->addStretch();

    // 右侧内容区
    auto *contentArea = new QWidget(page);
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(16);

    auto *title = new QLabel(tr("用户管理"), contentArea);
    title->setStyleSheet("font-size:20px; font-weight:700; color: #2c3e50;");

    // 搜索区域
    auto *searchLayout = new QHBoxLayout();
    m_userSearchInput = new QLineEdit(contentArea);
    m_userSearchInput->setPlaceholderText(tr("输入学号/工号或姓名搜索"));
    m_userSearchInput->setFixedWidth(300);
    auto *btnSearch = new QPushButton(tr("搜索"), contentArea);
    btnSearch->setFixedWidth(100);
    connect(btnSearch, &QPushButton::clicked, this, [this] { refreshUserManageData(); });
    connect(m_userSearchInput, &QLineEdit::returnPressed, this, [this] { refreshUserManageData(); });
    
    searchLayout->addWidget(m_userSearchInput);
    searchLayout->addWidget(btnSearch);
    searchLayout->addStretch();

    // 用户表格
    m_userTable = new QTableWidget(contentArea);
    m_userTable->setColumnCount(6);
    m_userTable->setHorizontalHeaderLabels({
        tr("学号/工号"), tr("姓名"), tr("角色"), tr("余额"), tr("状态"), tr("操作")
    });
    m_userTable->horizontalHeader()->setStretchLastSection(true);
    m_userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    contentLayout->addWidget(title);
    contentLayout->addLayout(searchLayout);
    contentLayout->addWidget(m_userTable, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    return page;
}

void AdminMainWindow::refreshUserManageData()
{
    if (!m_userTable || !DatabaseManager::init()) return;
    
    m_userTable->setRowCount(0);
    
    QString searchText = m_userSearchInput ? m_userSearchInput->text().trimmed() : QString();
    auto allUsers = DatabaseManager::fetchAllUsers();
    
    QStringList roleNames = {tr("学生"), tr("教职工"), tr(""), tr(""), tr(""), tr(""), tr(""), tr(""), tr(""), tr("管理员")};
    
    for (const auto &user : allUsers) {
        // 搜索过滤
        if (!searchText.isEmpty()) {
            if (!user.userId.contains(searchText, Qt::CaseInsensitive) && 
                !user.realName.contains(searchText, Qt::CaseInsensitive)) {
                continue;
            }
        }
        
        int row = m_userTable->rowCount();
        m_userTable->insertRow(row);
        
        m_userTable->setItem(row, 0, new QTableWidgetItem(user.userId));
        m_userTable->setItem(row, 1, new QTableWidgetItem(user.realName));
        m_userTable->setItem(row, 2, new QTableWidgetItem(
            user.role >= 0 && user.role < roleNames.size() ? roleNames[user.role] : tr("未知")));
        m_userTable->setItem(row, 3, new QTableWidgetItem(
            QString::number(user.credit, 'f', 2) + tr(" 元")));
        m_userTable->setItem(row, 4, new QTableWidgetItem(
            user.isActive ? tr("已激活") : tr("未激活")));
        
        // 操作按钮
        auto *btnResetPwd = new QPushButton(tr("重置密码"));
        btnResetPwd->setStyleSheet("font-size:12px; padding:4px 8px;");
        connect(btnResetPwd, &QPushButton::clicked, this, [this, user]() {
            QDialog dialog(this);
            dialog.setWindowTitle(tr("重置密码"));
            auto *layout = new QVBoxLayout(&dialog);
            
            auto *label = new QLabel(tr("用户: %1 (%2)").arg(user.userId).arg(user.realName));
            layout->addWidget(label);
            
            auto *inputPwd = new QLineEdit(&dialog);
            inputPwd->setPlaceholderText(tr("请输入新密码"));
            inputPwd->setEchoMode(QLineEdit::Password);
            layout->addWidget(inputPwd);
            
            auto *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
            connect(btnBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(btnBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
            layout->addWidget(btnBox);
            
            if (dialog.exec() == QDialog::Accepted) {
                QString newPassword = inputPwd->text();
                if (newPassword.length() < 6) {
                    QMessageBox::warning(this, tr("提示"), tr("密码长度至少为6位"));
                    return;
                }
                if (DatabaseManager::resetUserPassword(user.userId, newPassword)) {
                    QMessageBox::information(this, tr("成功"), tr("密码已重置"));
                } else {
                    QMessageBox::critical(this, tr("失败"), tr("重置失败，请重试"));
                }
            }
        });
        
        m_userTable->setCellWidget(row, 5, btnResetPwd);
    }
    
    m_userTable->resizeColumnsToContents();
}

QWidget* AdminMainWindow::createOrderManagePage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航栏
    auto *sidebar = new QWidget(page);
    sidebar->setMinimumWidth(200);
    sidebar->setMaximumWidth(200);
    sidebar->setStyleSheet(
        "QWidget { background-color: #34495e; }"
        "QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; }"
        "QPushButton:hover { background-color: #2c3e50; }"
    );
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    
    auto *btnDashboard = new QPushButton(tr("📊 首页概览"), sidebar);
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    
    auto *btnGear = new QPushButton(tr("☂️ 雨具管理"), sidebar);
    connect(btnGear, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    
    auto *btnUser = new QPushButton(tr("👤 用户管理"), sidebar);
    connect(btnUser, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    
    auto *btnOrder = new QPushButton(tr("📋 订单/流水"), sidebar);
    btnOrder->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: #2c3e50; }");
    connect(btnOrder, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });
    
    sidebarLayout->addWidget(btnDashboard);
    sidebarLayout->addWidget(btnGear);
    sidebarLayout->addWidget(btnUser);
    sidebarLayout->addWidget(btnOrder);
    sidebarLayout->addStretch();

    // 右侧内容区
    auto *contentArea = new QWidget(page);
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(16);

    auto *title = new QLabel(tr("订单/流水"), contentArea);
    title->setStyleSheet("font-size:20px; font-weight:700; color: #2c3e50;");

    // 订单表格
    m_orderTable = new QTableWidget(contentArea);
    m_orderTable->setColumnCount(6);
    m_orderTable->setHorizontalHeaderLabels({
        tr("流水号"), tr("用户"), tr("雨具ID"), tr("借出时间"), tr("归还时间"), tr("费用")
    });
    m_orderTable->horizontalHeader()->setStretchLastSection(true);
    m_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    contentLayout->addWidget(title);
    contentLayout->addWidget(m_orderTable, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    return page;
}

void AdminMainWindow::refreshOrderManageData()
{
    if (!m_orderTable || !DatabaseManager::init()) return;
    
    m_orderTable->setRowCount(0);
    
    // 查询所有借还记录
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) return;
    
    QSqlQuery query(db);
    query.prepare("SELECT record_id, user_id, gear_id, borrow_time, return_time, cost "
                  "FROM record ORDER BY borrow_time DESC LIMIT 100");
    
    if (!query.exec()) {
        qWarning() << "[Admin] 查询订单失败:" << query.lastError().text();
        return;
    }
    
    while (query.next()) {
        int row = m_orderTable->rowCount();
        m_orderTable->insertRow(row);
        
        m_orderTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        m_orderTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        m_orderTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        m_orderTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        m_orderTable->setItem(row, 4, new QTableWidgetItem(
            query.value(4).isNull() ? tr("未归还") : query.value(4).toString()));
        m_orderTable->setItem(row, 5, new QTableWidgetItem(
            QString::number(query.value(5).toDouble(), 'f', 2) + tr(" 元")));
    }
    
    m_orderTable->resizeColumnsToContents();
}
