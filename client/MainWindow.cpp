#include "MainWindow.h"
#include "SlotItem.h"
#include "Styles.h"
#include "Model/RainGearFactory.h"
#include "Model/RainGear_subclasses.hpp"

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    switchPage(Page::Welcome);
}

void MainWindow::setupUi()
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(createWelcomePage());     // 0 Welcome
    m_stack->addWidget(createCardReadPage());    // 1 CardRead
    m_stack->addWidget(createUserInputPage());   // 2 UserInput
    m_stack->addWidget(createFirstLoginPage());  // 3 FirstLogin
    m_stack->addWidget(createLoginPage());       // 4 Login
    m_stack->addWidget(createResetPwdPage());    // 5 ResetPwd
    m_stack->addWidget(createDashboardPage());   // 6 Dashboard
    m_stack->addWidget(createBorrowPage());      // 7 Borrow
    m_stack->addWidget(createMapPage());         // 8 Map
    m_stack->addWidget(createProfilePage());     // 9 Profile
    m_stack->addWidget(createInstructionPage()); // 10 Instruction

    layout->addWidget(m_stack);
    setCentralWidget(central);
}

QWidget* MainWindow::createWelcomePage()
{
    auto *page = new QWidget(this);
    page->setObjectName("welcomePage");
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(16);

    // 玻璃卡片容器
    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    card->setFixedSize(480, 420);
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setSpacing(20);
    cardLayout->setContentsMargins(40, 50, 40, 50);

    // 图标
    auto *iconLabel = new QLabel("☂️", card);
    iconLabel->setStyleSheet(Styles::Labels::welcomeIcon());
    iconLabel->setAlignment(Qt::AlignCenter);

    // 主标题
    auto *title = new QLabel(tr("NUIST 智能雨具系统"), card);
    title->setStyleSheet(Styles::Labels::title());
    title->setAlignment(Qt::AlignCenter);

    // 副标题
    auto *subtitle = new QLabel(tr("校园智能共享雨具借还平台"), card);
    subtitle->setStyleSheet(Styles::Labels::subtitle());
    subtitle->setAlignment(Qt::AlignCenter);

    // 开始按钮
    auto *btnStart = new QPushButton(tr("开始使用"), card);
    btnStart->setStyleSheet(Styles::Buttons::primaryLarge());
    btnStart->setCursor(Qt::PointingHandCursor);
    connect(btnStart, &QPushButton::clicked, this, [this] {
        switchPage(Page::CardRead);
    });

    cardLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    cardLayout->addWidget(title, 0, Qt::AlignCenter);
    cardLayout->addWidget(subtitle, 0, Qt::AlignCenter);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(btnStart, 0, Qt::AlignCenter);

    layout->addWidget(card, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createCardReadPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    // 玻璃卡片容器
    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    card->setFixedSize(500, 450);
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setSpacing(20);
    cardLayout->setContentsMargins(40, 40, 40, 40);

    auto *iconLabel = new QLabel("💳", card);
    iconLabel->setStyleSheet(Styles::Labels::welcomeIcon());
    iconLabel->setAlignment(Qt::AlignCenter);

    auto *tip = new QLabel(tr("请将您的一卡通放置在刷卡处"), card);
    tip->setStyleSheet(Styles::Labels::pageTitle());
    tip->setAlignment(Qt::AlignCenter);

    auto *subtip = new QLabel(tr("系统将自动识别您的学号和姓名"), card);
    subtip->setStyleSheet(Styles::Labels::subtitle());
    subtip->setAlignment(Qt::AlignCenter);

    // 模拟刷卡动画指示
    auto *hintLabel = new QLabel(tr("（演示模式：点击确定手动输入信息）"), card);
    hintLabel->setStyleSheet(Styles::Labels::hint());
    hintLabel->setAlignment(Qt::AlignCenter);

    auto *btnConfirm = new QPushButton(tr("确定"), card);
    btnConfirm->setStyleSheet(Styles::Buttons::primary());
    btnConfirm->setCursor(Qt::PointingHandCursor);
    connect(btnConfirm, &QPushButton::clicked, this, [this] {
        switchPage(Page::UserInput);
    });

    auto *btnBack = new QPushButton(tr("返回首页"), card);
    btnBack->setStyleSheet(Styles::Buttons::back());
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Welcome);
    });

    cardLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    cardLayout->addWidget(tip, 0, Qt::AlignCenter);
    cardLayout->addWidget(subtip, 0, Qt::AlignCenter);
    cardLayout->addWidget(hintLabel, 0, Qt::AlignCenter);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(btnConfirm, 0, Qt::AlignCenter);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(btnBack, 0, Qt::AlignCenter);

    layout->addWidget(card, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createUserInputPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    // 玻璃卡片容器
    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    card->setFixedSize(480, 450);
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setSpacing(20);
    cardLayout->setContentsMargins(50, 40, 50, 40);

    auto *title = new QLabel(tr("请输入您的信息"), card);
    title->setStyleSheet(Styles::Labels::pageTitle());
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(tr("演示模式：请手动输入学号/工号和姓名"), card);
    subtitle->setStyleSheet(Styles::Labels::hint());
    subtitle->setAlignment(Qt::AlignCenter);

    // 使用成员变量保存输入框指针
    if (!m_inputUser) {
        m_inputUser = new QLineEdit(card);
    }
    if (!m_inputName) {
        m_inputName = new QLineEdit(card);
    }
    m_inputUser->setPlaceholderText(tr("请输入学号/工号"));
    m_inputName->setPlaceholderText(tr("请输入姓名"));
    m_inputUser->setFixedWidth(320);
    m_inputName->setFixedWidth(320);
    m_inputUser->clear();
    m_inputName->clear();

    auto *btnSubmit = new QPushButton(tr("提交验证"), card);
    btnSubmit->setStyleSheet(Styles::Buttons::primary());
    btnSubmit->setCursor(Qt::PointingHandCursor);
    connect(btnSubmit, &QPushButton::clicked, this, [this] {
        const QString userId = m_inputUser->text().trimmed();
        const QString realName = m_inputName->text().trimmed();

        if (userId.isEmpty() || realName.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("请输入学号/工号和姓名"));
            return;
        }

        if (!DatabaseManager::init()) {
            QMessageBox::critical(this, tr("数据库错误"), tr("无法连接到本地 MySQL，请检查服务是否已启动。"));
            return;
        }

        qDebug() << "[UserInput] 查询用户:" << userId << realName;

        auto record = DatabaseManager::fetchUserByIdAndName(userId, realName);
        if (!record) {
            QMessageBox::warning(this, tr("用户不存在"), tr("未找到该学号/工号和姓名对应的用户，请检查输入。"));
            return;
        }

        qDebug() << "[UserInput] 找到用户, is_active:" << record->isActive;

        m_tempUserId = userId;
        m_tempUserName = realName;
        m_currentRole = (record->role == 1) ? Role::Staff : Role::Student;

        if (!record->isActive) {
            qDebug() << "[UserInput] 首次登录，跳转到设置密码页面";
            switchPage(Page::FirstLogin);
        } else {
            qDebug() << "[UserInput] 非首次登录，跳转到密码登录页面";
            switchPage(Page::Login);
        }
    });

    auto *btnBack = new QPushButton(tr("返回"), card);
    btnBack->setStyleSheet(Styles::Buttons::back());
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::CardRead);
    });

    cardLayout->addWidget(title, 0, Qt::AlignCenter);
    cardLayout->addWidget(subtitle, 0, Qt::AlignCenter);
    cardLayout->addSpacing(16);
    cardLayout->addWidget(m_inputUser, 0, Qt::AlignCenter);
    cardLayout->addWidget(m_inputName, 0, Qt::AlignCenter);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(btnSubmit, 0, Qt::AlignCenter);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(btnBack, 0, Qt::AlignCenter);

    layout->addWidget(card, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createFirstLoginPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    // 玻璃卡片容器
    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    card->setFixedSize(480, 520);
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setSpacing(16);
    cardLayout->setContentsMargins(50, 40, 50, 40);

    auto *title = new QLabel(tr("🎉 首次登录"), card);
    title->setStyleSheet(Styles::Labels::pageTitle());
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(tr("欢迎加入！请设置您的登录密码"), card);
    subtitle->setStyleSheet(Styles::Labels::subtitle());
    subtitle->setAlignment(Qt::AlignCenter);

    // 用户信息显示区
    m_firstLoginUserInfoLabel = new QLabel(card);
    m_firstLoginUserInfoLabel->setStyleSheet(Styles::Labels::info());
    m_firstLoginUserInfoLabel->setAlignment(Qt::AlignCenter);
    QString infoText = tr("账号：%1 | 姓名：%2")
        .arg(m_tempUserId.isEmpty() ? tr("未知") : m_tempUserId)
        .arg(m_tempUserName.isEmpty() ? tr("未知") : m_tempUserName);
    m_firstLoginUserInfoLabel->setText(infoText);
    
    m_inputNewPass = new QLineEdit(card);
    m_inputConfirmPass = new QLineEdit(card);
    m_inputNewPass->setPlaceholderText(tr("请输入新密码（至少6位）"));
    m_inputConfirmPass->setPlaceholderText(tr("请再次确认密码"));
    m_inputNewPass->setEchoMode(QLineEdit::Password);
    m_inputConfirmPass->setEchoMode(QLineEdit::Password);
    m_inputNewPass->setFixedWidth(320);
    m_inputConfirmPass->setFixedWidth(320);

    auto *btnSubmit = new QPushButton(tr("完成注册"), card);
    btnSubmit->setStyleSheet(Styles::Buttons::accent());
    btnSubmit->setCursor(Qt::PointingHandCursor);
    connect(btnSubmit, &QPushButton::clicked, this, [this] {
        if (performFirstLogin()) {
            QMessageBox::information(this, tr("注册成功"), tr("密码设置成功！请使用新密码登录。"));
            switchPage(Page::Login);
        }
    });

    auto *btnBack = new QPushButton(tr("返回"), card);
    btnBack->setStyleSheet(Styles::Buttons::back());
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::UserInput);
    });

    cardLayout->addWidget(title, 0, Qt::AlignCenter);
    cardLayout->addWidget(subtitle, 0, Qt::AlignCenter);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(m_firstLoginUserInfoLabel, 0, Qt::AlignCenter);
    cardLayout->addSpacing(12);
    cardLayout->addWidget(m_inputNewPass, 0, Qt::AlignCenter);
    cardLayout->addWidget(m_inputConfirmPass, 0, Qt::AlignCenter);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(btnSubmit, 0, Qt::AlignCenter);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(btnBack, 0, Qt::AlignCenter);

    layout->addWidget(card, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createLoginPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    // 玻璃卡片容器
    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    card->setFixedSize(480, 480);
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setSpacing(16);
    cardLayout->setContentsMargins(50, 40, 50, 40);

    m_loginRoleLabel = new QLabel(tr("🔐 账号登录"), card);
    m_loginRoleLabel->setStyleSheet(Styles::Labels::pageTitle());
    m_loginRoleLabel->setAlignment(Qt::AlignCenter);

    // 用户信息显示
    auto *userInfoLabel = new QLabel(card);
    userInfoLabel->setStyleSheet(Styles::Labels::info());
    userInfoLabel->setAlignment(Qt::AlignCenter);
    QString infoText = tr("账号：%1 | 姓名：%2")
        .arg(m_tempUserId.isEmpty() ? tr("未知") : m_tempUserId)
        .arg(m_tempUserName.isEmpty() ? tr("未知") : m_tempUserName);
    userInfoLabel->setText(infoText);
    m_loginUserInfoLabel = userInfoLabel;
    
    m_inputPass = new QLineEdit(card);
    m_inputPass->setEchoMode(QLineEdit::Password);
    m_inputPass->setPlaceholderText(tr("请输入密码"));
    m_inputPass->setFixedWidth(320);

    auto *btnLogin = new QPushButton(tr("登录"), card);
    btnLogin->setStyleSheet(Styles::Buttons::primary());
    btnLogin->setCursor(Qt::PointingHandCursor);
    connect(btnLogin, &QPushButton::clicked, this, [this] {
        if (m_tempUserId.isEmpty() || m_tempUserName.isEmpty()) {
            QMessageBox::warning(this, tr("错误"), tr("用户信息丢失，请重新输入。"));
            switchPage(Page::UserInput);
            return;
        }

        const QString password = m_inputPass ? m_inputPass->text() : QString();
        if (password.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("请输入密码"));
            return;
        }

        if (!DatabaseManager::init()) {
            QMessageBox::critical(this, tr("数据库错误"), tr("无法连接到本地 MySQL，请检查服务是否已启动。"));
            return;
        }

        auto record = DatabaseManager::fetchUserByIdAndNameAndPassword(m_tempUserId, m_tempUserName, password);
        if (!record) {
            QMessageBox::warning(this, tr("登录失败"), tr("密码错误，请检查输入。"));
            return;
        }

        m_currentRole = (record->role == 1) ? Role::Staff : Role::Student;
        m_currentUser = std::make_unique<User>(record->userId, record->realName, record->credit, record->role);

        updateRoleLabel();
        updateProfileFromUser();
        QMessageBox::information(this, tr("登录成功"), tr("欢迎回来！"));
        switchPage(Page::Dashboard);
    });

    auto *btnReset = new QPushButton(tr("忘记密码？修改密码"), card);
    btnReset->setStyleSheet(Styles::Buttons::link());
    btnReset->setCursor(Qt::PointingHandCursor);
    connect(btnReset, &QPushButton::clicked, this, [this] {
        switchPage(Page::ResetPwd);
    });

    auto *btnBack = new QPushButton(tr("返回"), card);
    btnBack->setStyleSheet(Styles::Buttons::back());
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::UserInput);
    });

    cardLayout->addWidget(m_loginRoleLabel, 0, Qt::AlignCenter);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(userInfoLabel, 0, Qt::AlignCenter);
    cardLayout->addSpacing(16);
    cardLayout->addWidget(m_inputPass, 0, Qt::AlignCenter);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(btnLogin, 0, Qt::AlignCenter);
    cardLayout->addWidget(btnReset, 0, Qt::AlignCenter);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(btnBack, 0, Qt::AlignCenter);

    layout->addWidget(card, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createResetPwdPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    // 玻璃卡片容器
    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    card->setFixedSize(480, 500);
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setSpacing(14);
    cardLayout->setContentsMargins(50, 40, 50, 40);

    auto *title = new QLabel(tr("🔑 修改密码"), card);
    title->setStyleSheet(Styles::Labels::pageTitle());
    title->setAlignment(Qt::AlignCenter);

    auto *inputOld = new QLineEdit(card);
    inputOld->setPlaceholderText(tr("请输入旧密码"));
    inputOld->setEchoMode(QLineEdit::Password);
    inputOld->setFixedWidth(320);
    
    auto *inputNew = new QLineEdit(card);
    inputNew->setPlaceholderText(tr("请输入新密码（至少6位）"));
    inputNew->setEchoMode(QLineEdit::Password);
    inputNew->setFixedWidth(320);
    
    auto *inputConfirm = new QLineEdit(card);
    inputConfirm->setPlaceholderText(tr("请再次确认新密码"));
    inputConfirm->setEchoMode(QLineEdit::Password);
    inputConfirm->setFixedWidth(320);

    auto *btnSubmit = new QPushButton(tr("确认修改"), card);
    btnSubmit->setStyleSheet(Styles::Buttons::primary());
    btnSubmit->setCursor(Qt::PointingHandCursor);
    connect(btnSubmit, &QPushButton::clicked, this, [this, inputOld, inputNew, inputConfirm] {
        const QString oldPassword = inputOld->text();
        const QString newPassword = inputNew->text();
        const QString confirmPassword = inputConfirm->text();

        if (oldPassword.isEmpty() || newPassword.isEmpty() || confirmPassword.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("请填写所有密码字段"));
            return;
        }

        if (newPassword != confirmPassword) {
            QMessageBox::warning(this, tr("密码不一致"), tr("两次输入的新密码不一致，请重新输入。"));
            return;
        }

        if (newPassword.length() < 6) {
            QMessageBox::warning(this, tr("密码太短"), tr("密码长度至少为6位，请重新输入。"));
            return;
        }

        const QString userId = m_tempUserId;
        if (userId.isEmpty()) {
            QMessageBox::warning(this, tr("错误"), tr("用户信息丢失，请返回上一页重新输入。"));
            switchPage(Page::UserInput);
            return;
        }

        if (!DatabaseManager::changeUserPassword(userId, oldPassword, newPassword)) {
            QMessageBox::warning(this, tr("修改失败"), tr("旧密码错误或修改失败，请重试。"));
            return;
        }

        if (m_inputPass) {
            m_inputPass->clear();
        }
        QMessageBox::information(this, tr("修改成功"), tr("密码已成功修改，请使用新密码重新登录。"));
        switchPage(Page::Login);
    });

    auto *btnBack = new QPushButton(tr("返回"), card);
    btnBack->setStyleSheet(Styles::Buttons::back());
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Login);
    });

    cardLayout->addWidget(title, 0, Qt::AlignCenter);
    cardLayout->addSpacing(16);
    cardLayout->addWidget(inputOld, 0, Qt::AlignCenter);
    cardLayout->addWidget(inputNew, 0, Qt::AlignCenter);
    cardLayout->addWidget(inputConfirm, 0, Qt::AlignCenter);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(btnSubmit, 0, Qt::AlignCenter);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(btnBack, 0, Qt::AlignCenter);

    layout->addWidget(card, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createDashboardPage()
{
    auto *page = new QWidget(this);
    auto *outerLayout = new QVBoxLayout(page);
    outerLayout->setContentsMargins(24, 24, 24, 24);
    outerLayout->setSpacing(16);

    // 玻璃卡片容器
    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 24, 32, 24);
    cardLayout->setSpacing(20);

    // 顶部栏：标题和退出登录按钮
    auto *topBar = new QHBoxLayout();
    topBar->setContentsMargins(0, 0, 0, 0);
    
    auto *title = new QLabel(tr("☂️ NUIST 智能雨具系统"), card);
    title->setStyleSheet(Styles::Labels::pageTitle());
    
    auto *btnLogout = new QPushButton(tr("退出登录"), card);
    btnLogout->setStyleSheet(Styles::Buttons::logout());
    btnLogout->setCursor(Qt::PointingHandCursor);
    
    connect(btnLogout, &QPushButton::clicked, this, [this] {
        m_currentUser.reset();
        m_tempUserId.clear();
        m_tempUserName.clear();
        m_currentRole = Role::Unknown;
        m_currentStationId = 0;
        
        if (m_inputPass) m_inputPass->clear();
        if (m_inputNewPass) m_inputNewPass->clear();
        if (m_inputConfirmPass) m_inputConfirmPass->clear();
        if (m_stationComboBox) m_stationComboBox->setCurrentIndex(0);
        if (m_inputUser) m_inputUser->clear();
        if (m_inputName) m_inputName->clear();
        
        switchPage(Page::Welcome);
    });
    
    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(btnLogout);
    
    // 站点选择区域（无背景，直接显示）
    auto *stationContainer = new QWidget(card);
    stationContainer->setStyleSheet("background: transparent;");
    auto *stationLayout = new QHBoxLayout(stationContainer);
    stationLayout->setContentsMargins(0, 8, 0, 8);
    stationLayout->setSpacing(12);
    
    auto *stationLabel = new QLabel(tr("📍 当前站点："), stationContainer);
    stationLabel->setStyleSheet("font-size: 15px; font-weight: 600; color: #4a4a68; background: transparent;");
    
    m_stationComboBox = new QComboBox(stationContainer);
    m_stationComboBox->setFixedWidth(220);
    loadStations();
    
    connect(m_stationComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::onStationChanged);
    
    stationLayout->addStretch();
    stationLayout->addWidget(stationLabel);
    stationLayout->addWidget(m_stationComboBox);
    stationLayout->addStretch();
    
    // 主功能按钮区
    auto *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(24);
    buttonsLayout->setAlignment(Qt::AlignCenter);

    auto *btnBorrow = new QPushButton(tr("☔ 我要借伞"), card);
    auto *btnReturn = new QPushButton(tr("🔄 我要还伞"), card);
    btnBorrow->setStyleSheet(Styles::Buttons::feature());
    btnReturn->setStyleSheet(Styles::Buttons::feature());
    btnBorrow->setCursor(Qt::PointingHandCursor);
    btnReturn->setCursor(Qt::PointingHandCursor);

    connect(btnBorrow, &QPushButton::clicked, this, [this] {
        if (m_currentStationId == 0) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择站点"));
            return;
        }
        m_borrowMode = true;
        switchPage(Page::Borrow);
    });
    connect(btnReturn, &QPushButton::clicked, this, [this] {
        if (m_currentStationId == 0) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择站点"));
            return;
        }
        m_borrowMode = false;
        switchPage(Page::Borrow);
    });

    buttonsLayout->addWidget(btnBorrow);
    buttonsLayout->addWidget(btnReturn);

    // 使用说明链接
    auto *btnInstruction = new QPushButton(tr("📖 查看使用说明与收费标准"), card);
    btnInstruction->setStyleSheet(Styles::Buttons::link());
    btnInstruction->setCursor(Qt::PointingHandCursor);
    connect(btnInstruction, &QPushButton::clicked, this, [this] {
        switchPage(Page::Instruction);
    });

    // 底部导航
    auto *bottom = new QHBoxLayout();
    bottom->setContentsMargins(0, 16, 0, 0);
    bottom->setSpacing(16);
    
    auto *btnProfile = new QPushButton(tr("👤 个人中心"), card);
    btnProfile->setStyleSheet(Styles::Buttons::nav());
    btnProfile->setCursor(Qt::PointingHandCursor);
    
    auto *btnMap = new QPushButton(tr("🗺️ 站点地图"), card);
    btnMap->setStyleSheet(Styles::Buttons::nav());
    btnMap->setCursor(Qt::PointingHandCursor);
    
    connect(btnProfile, &QPushButton::clicked, this, [this] {
        updateProfileFromUser();
        switchPage(Page::Profile);
    });
    connect(btnMap, &QPushButton::clicked, this, [this] {
        switchPage(Page::Map);
    });
    
    bottom->addWidget(btnProfile);
    bottom->addStretch();
    bottom->addWidget(btnMap);

    // 组装卡片布局
    cardLayout->addLayout(topBar);
    cardLayout->addWidget(stationContainer);
    cardLayout->addStretch();
    cardLayout->addLayout(buttonsLayout);
    cardLayout->addWidget(btnInstruction, 0, Qt::AlignCenter);
    cardLayout->addStretch();
    cardLayout->addLayout(bottom);

    outerLayout->addWidget(card);
    
    return page;
}

QWidget* MainWindow::createBorrowPage()
{
    auto *page = new QWidget(this);
    auto *outer = new QVBoxLayout(page);
    outer->setContentsMargins(24, 24, 24, 24);
    outer->setSpacing(16);

    // 玻璃卡片容器
    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 20, 24, 20);
    cardLayout->setSpacing(16);

    auto *topBar = new QHBoxLayout();
    m_slotTitle = new QLabel(tr("☔ 借伞模式"), card);
    m_slotTitle->setStyleSheet(Styles::Labels::pageTitle());
    
    auto *btnBack = new QPushButton(tr("返回主页"), card);
    btnBack->setStyleSheet(Styles::Buttons::back());
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    topBar->addWidget(m_slotTitle);
    topBar->addStretch();
    topBar->addWidget(btnBack);
    cardLayout->addLayout(topBar);

    // 提示信息
    auto *hintLabel = new QLabel(card);
    hintLabel->setStyleSheet(Styles::Labels::hint());
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setText(tr("🟢 绿色=可借  ⚪ 灰色=空槽可还  🔴 红色=故障"));
    cardLayout->addWidget(hintLabel);

    auto *grid = new QGridLayout();
    grid->setSpacing(14);

    // Create 12 slot widgets (3x4)
    for (int i = 0; i < 12; ++i) {
        auto *slot = new SlotItem(i, card);
        slot->setState(SlotItem::State::Empty);
        slot->setIcon(QPixmap(), QStringLiteral("#%1").arg(i + 1));
        
        connect(slot, &SlotItem::clicked, this, [this, i](int index, SlotItem::State state) {
            if (m_currentStationId == 0) {
                QMessageBox::warning(this, tr("错误"), tr("请先选择站点"));
                return;
            }
            
            if (!m_currentUser) {
                QMessageBox::warning(this, tr("错误"), tr("请先登录"));
                switchPage(Page::Dashboard);
                return;
            }
            
            int slotId = index + 1;
            
            if (m_borrowMode) {
                handleBorrowGear(slotId, index);
            } else {
                handleReturnGear(slotId, index);
            }
        });
        m_slots.push_back(slot);
        grid->addWidget(slot, i / 4, i % 4);
    }

    cardLayout->addLayout(grid);
    outer->addWidget(card);
    return page;
}

QWidget* MainWindow::createMapPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    // 玻璃卡片容器
    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 20, 24, 20);
    cardLayout->setSpacing(16);

    // 顶部标题栏
    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel(tr("🗺️ 校园雨具站点分布图"), card);
    title->setStyleSheet(Styles::Labels::pageTitle());
    auto *btnBack = new QPushButton(tr("返回主页"), card);
    btnBack->setStyleSheet(Styles::Buttons::back());
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });
    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(btnBack);

    // 图例说明
    auto *legendLabel = new QLabel(tr("🟢 库存充足(≥5)  🟡 库存紧张(2-4)  🔴 库存不足(<2)"), card);
    legendLabel->setStyleSheet(Styles::Labels::hint());
    legendLabel->setAlignment(Qt::AlignCenter);

    // 地图容器
    auto *mapContainer = new QWidget(card);
    mapContainer->setMinimumSize(750, 500);
    mapContainer->setStyleSheet(Styles::mapContainer());
    
    loadMapStations(mapContainer);

    cardLayout->addLayout(topBar);
    cardLayout->addWidget(legendLabel);
    cardLayout->addWidget(mapContainer, 1);
    
    layout->addWidget(card);
    return page;
}

QWidget* MainWindow::createProfilePage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    // 玻璃卡片容器
    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    card->setFixedSize(450, 420);
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setSpacing(16);
    cardLayout->setContentsMargins(40, 40, 40, 40);

    // 头像区域
    auto *avatarLabel = new QLabel("👤", card);
    avatarLabel->setStyleSheet("font-size: 64px;");
    avatarLabel->setAlignment(Qt::AlignCenter);

    m_profileTitle = new QLabel(tr("个人中心"), card);
    m_profileTitle->setStyleSheet(Styles::Labels::pageTitle());
    m_profileTitle->setAlignment(Qt::AlignCenter);
    
    m_profileName = new QLabel(card);
    m_profileName->setStyleSheet(Styles::Labels::info());
    m_profileName->setAlignment(Qt::AlignCenter);
    
    m_profileId = new QLabel(card);
    m_profileId->setStyleSheet(Styles::Labels::hint());
    m_profileId->setAlignment(Qt::AlignCenter);
    
    m_profileBalance = new QLabel(card);
    m_profileBalance->setStyleSheet(Styles::Labels::balance());
    m_profileBalance->setAlignment(Qt::AlignCenter);

    // 按钮区域
    auto *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(16);
    
    auto *btnRefresh = new QPushButton(tr("🔄 刷新余额"), card);
    btnRefresh->setStyleSheet(Styles::Buttons::secondary());
    btnRefresh->setCursor(Qt::PointingHandCursor);
    connect(btnRefresh, &QPushButton::clicked, this, [this] {
        updateProfileFromUser();
        QMessageBox::information(this, tr("提示"), tr("余额已刷新"));
    });
    
    auto *btnBack = new QPushButton(tr("返回主页"), card);
    btnBack->setStyleSheet(Styles::Buttons::back());
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });
    
    btnLayout->addWidget(btnRefresh);
    btnLayout->addWidget(btnBack);

    cardLayout->addWidget(avatarLabel, 0, Qt::AlignCenter);
    cardLayout->addWidget(m_profileTitle, 0, Qt::AlignCenter);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(m_profileName, 0, Qt::AlignCenter);
    cardLayout->addWidget(m_profileId, 0, Qt::AlignCenter);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(m_profileBalance, 0, Qt::AlignCenter);
    cardLayout->addSpacing(20);
    cardLayout->addLayout(btnLayout);

    layout->addWidget(card, 0, Qt::AlignCenter);
    updateProfileFromUser();
    return page;
}

QWidget* MainWindow::createInstructionPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    // 玻璃卡片容器
    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 20, 24, 20);
    cardLayout->setSpacing(16);

    // 顶部标题栏
    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel(tr("📖 使用说明与服务协议"), card);
    title->setStyleSheet(Styles::Labels::pageTitle());
    auto *btnBack = new QPushButton(tr("返回主页"), card);
    btnBack->setStyleSheet(Styles::Buttons::back());
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });
    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(btnBack);

    m_instructionViewer = new QTextBrowser(card);
    m_instructionViewer->setOpenExternalLinks(false);
    m_instructionViewer->setHtml(
        "<div style='font-family: Microsoft YaHei UI, sans-serif; color: #1a1a2e; line-height: 1.8;'>"
        "<h2 style='text-align:center; color:#667eea;'>☂️ NUIST 智能雨具系统服务协议</h2>"
        "<hr style='border: 1px solid #e4e9f2;'>"
        "<h3 style='color:#764ba2;'>一、服务对象</h3>"
        "<p>本系统仅面向 <b>NUIST 在校教职工与学生</b> 开放，登录需验证校园一卡通账户。</p>"
        "<h3 style='color:#764ba2;'>二、借还规则</h3>"
        "<p><b>🔹 借出：</b>账户余额需大于相应押金方可借用。借出时系统将冻结相应金额作为押金。</p>"
        "<p><b>🔹 归还：</b>请将雨具插入任意站点的空闲卡槽，看到屏幕提示【归还成功】后方可离开。</p>"
        "<h3 style='color:#764ba2;'>三、资费标准</h3>"
        "<table style='width:100%; border-collapse:collapse; margin:10px 0;'>"
        "<tr style='background:#f8f9ff;'><td style='padding:8px; border:1px solid #e4e9f2;'><b>雨具类型</b></td><td style='padding:8px; border:1px solid #e4e9f2;'><b>押金</b></td></tr>"
        "<tr><td style='padding:8px; border:1px solid #e4e9f2;'>普通塑料伞</td><td style='padding:8px; border:1px solid #e4e9f2;'>10 元</td></tr>"
        "<tr><td style='padding:8px; border:1px solid #e4e9f2;'>高质量抗风伞</td><td style='padding:8px; border:1px solid #e4e9f2;'>20 元</td></tr>"
        "<tr><td style='padding:8px; border:1px solid #e4e9f2;'>专用遮阳伞</td><td style='padding:8px; border:1px solid #e4e9f2;'>15 元</td></tr>"
        "<tr><td style='padding:8px; border:1px solid #e4e9f2;'>雨衣</td><td style='padding:8px; border:1px solid #e4e9f2;'>25 元</td></tr>"
        "</table>"
        "<p>⏰ <b>免费时长：</b>借出后 <b style='color:#00d68f;'>24小时内</b> 归还免费。</p>"
        "<p>💰 <b>超时费用：</b>超过24小时，按 <b style='color:#ff3d71;'>1元/12小时</b> 从余额扣除。</p>"
        "<h3 style='color:#764ba2;'>四、遗失与损坏</h3>"
        "<p>若雨具遗失或严重损坏导致无法归还，系统将<b style='color:#ff3d71;'>扣除全额押金</b>用于赔偿。</p>"
        "<h3 style='color:#764ba2;'>五、联系我们</h3>"
        "<p>如遇设备故障或扣费异常，请致电校园服务中心：<b>5873-6110</b></p>"
        "</div>"
    );

    auto *btnConfirm = new QPushButton(tr("✓ 我已阅读并同意"), card);
    btnConfirm->setStyleSheet(Styles::Buttons::primary());
    btnConfirm->setCursor(Qt::PointingHandCursor);
    connect(btnConfirm, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    cardLayout->addLayout(topBar);
    cardLayout->addWidget(m_instructionViewer, 1);
    cardLayout->addWidget(btnConfirm, 0, Qt::AlignCenter);

    layout->addWidget(card);
    return page;
}

void MainWindow::switchPage(Page page)
{
    m_stack->setCurrentIndex(static_cast<int>(page));
    if (page == Page::Borrow && m_slotTitle) {
        m_slotTitle->setText(m_borrowMode ? tr("☔ 借伞模式") : tr("🔄 还伞模式"));
        // 切换到借还页面时，如果已选择站点，刷新槽位状态
        if (m_currentStationId > 0) {
            refreshSlotsFromDatabase();
            populateSlots(m_borrowMode);
        } else {
            // 如果没有选择站点，先显示空槽位
            for (auto *slot : m_slots) {
                slot->setState(SlotItem::State::Empty);
                slot->setEnabled(false);
            }
        }
    } else if (page == Page::Login && m_loginUserInfoLabel) {
        // 切换到密码登录页面时，更新显示的用户信息
        QString infoText = tr("学号/工号：%1\n姓名：%2")
            .arg(m_tempUserId.isEmpty() ? tr("未知") : m_tempUserId)
            .arg(m_tempUserName.isEmpty() ? tr("未知") : m_tempUserName);
        m_loginUserInfoLabel->setText(infoText);
    } else if (page == Page::FirstLogin && m_firstLoginUserInfoLabel) {
        // 切换到首次登录页面时，更新显示的用户信息
        QString infoText = tr("学号/工号：%1\n姓名：%2")
            .arg(m_tempUserId.isEmpty() ? tr("未知") : m_tempUserId)
            .arg(m_tempUserName.isEmpty() ? tr("未知") : m_tempUserName);
        m_firstLoginUserInfoLabel->setText(infoText);
    } else if (page == Page::UserInput) {
        // 切换到用户输入页面时，清空输入框
        if (m_inputUser) {
            m_inputUser->clear();
        }
        if (m_inputName) {
            m_inputName->clear();
        }
    }
}

void MainWindow::populateSlots(bool borrowMode)
{
    // 如果未选择站点，提示用户
    if (m_currentStationId == 0) {
        QMessageBox::warning(this, tr("提示"), tr("请先在主页面选择站点"));
        switchPage(Page::Dashboard);
        return;
    }
    
    // 从数据库刷新槽位状态（颜色和类型完全由 refreshSlotsFromDatabase 控制）
    refreshSlotsFromDatabase();
    // 不再通过 setEnabled(true/false) 灰掉控件，否则图标会显得"糊"；
    // 借伞/还伞模式的可点击逻辑在 handleBorrowGear / handleReturnGear 中根据状态再做检查。
    
    // 确保UI立即更新
    QApplication::processEvents();
}

void MainWindow::updateRoleLabel()
{
    if (!m_loginRoleLabel) return;
    QString roleText = tr("请选择身份");
    if (m_currentRole == Role::Student) roleText = tr("学生登录");
    else if (m_currentRole == Role::Staff) roleText = tr("教职人员登录");
    m_loginRoleLabel->setText(roleText);
}

void MainWindow::updateProfileFromUser()
{
    if (!m_profileName || !m_profileId || !m_profileBalance || !m_profileTitle) return;

    if (!m_currentUser) {
        m_profileTitle->setText(tr("个人中心"));
        m_profileName->setText(tr("姓名：未登录"));
        m_profileId->setText(tr("账号：-"));
        m_profileBalance->setText(tr("￥0.00"));
        m_profileBalance->setStyleSheet("font-size:28px; font-weight:700; color:#8f8fa3;");
        return;
    }

    const bool isStaff = m_currentUser->get_role() == 1;
    m_profileTitle->setText(tr("个人中心"));
    m_profileName->setText(tr("👋 %1").arg(m_currentUser->get_name()));
    m_profileId->setText(isStaff ? tr("工号：%1").arg(m_currentUser->get_id())
                                 : tr("学号：%1").arg(m_currentUser->get_id()));
    m_profileBalance->setText(tr("💰 ￥%1").arg(QString::number(m_currentUser->get_balance(), 'f', 2)));
    m_profileBalance->setStyleSheet(Styles::Labels::balance());
}

bool MainWindow::performUserInput()
{
    // 这个函数现在不再使用，逻辑已移到createUserInputPage的lambda中
    // 保留此函数以防其他地方调用
    QMessageBox::warning(this, tr("错误"), tr("此函数已废弃，请使用页面内的提交逻辑"));
    return false;
}

bool MainWindow::performFirstLogin()
{
    const QString newPassword = m_inputNewPass ? m_inputNewPass->text() : QString();
    const QString confirmPassword = m_inputConfirmPass ? m_inputConfirmPass->text() : QString();

    if (newPassword.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请输入新密码和确认密码"));
        return false;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, tr("密码不一致"), tr("两次输入的密码不一致，请重新输入。"));
        return false;
    }

    if (newPassword.length() < 6) {
        QMessageBox::warning(this, tr("密码太短"), tr("密码长度至少为6位，请重新输入。"));
        return false;
    }

    if (m_tempUserId.isEmpty()) {
        QMessageBox::critical(this, tr("错误"), tr("用户信息丢失，请重新输入。"));
        return false;
    }

    // 更新密码并激活账户
    if (!DatabaseManager::updateUserPassword(m_tempUserId, newPassword)) {
        QMessageBox::critical(this, tr("设置失败"), tr("密码设置失败，请重试。"));
        return false;
    }

    // 清空密码输入框，准备重新登录
    if (m_inputNewPass) {
        m_inputNewPass->clear();
    }
    if (m_inputConfirmPass) {
        m_inputConfirmPass->clear();
    }
    if (m_inputPass) {
        m_inputPass->clear();
    }

    return true;
}

bool MainWindow::performLogin()
{
    // 这个函数现在不再使用，登录逻辑已移到createLoginPage的lambda中
    // 保留此函数以防其他地方调用
    QMessageBox::warning(this, tr("错误"), tr("此函数已废弃，请使用页面内的登录逻辑"));
    return false;
}

void MainWindow::loadStations()
{
    if (!m_stationComboBox) return;
    
    m_stationComboBox->clear();
    m_stationComboBox->addItem(tr("-- 请选择站点 --"), 0);
    
    if (!DatabaseManager::init()) {
        qWarning() << "[MainWindow] 数据库连接失败，无法加载站点";
        return;
    }
    
    auto stations = DatabaseManager::fetchAllStations();
    for (const auto &station : stations) {
        m_stationComboBox->addItem(station.name, station.stationId);
    }
    
    qDebug() << "[MainWindow] 已加载" << stations.size() << "个站点";
}

void MainWindow::onStationChanged(int index)
{
    if (!m_stationComboBox || index < 0) return;
    
    int stationId = m_stationComboBox->itemData(index).toInt();
    if (stationId == 0) {
        m_currentStationId = 0;
        return;
    }
    
    m_currentStationId = stationId;
    qDebug() << "[MainWindow] 站点已切换为:" << m_stationComboBox->itemText(index) << "ID:" << stationId;
    
    // 如果当前在借还页面，刷新槽位状态
    if (m_stack && m_stack->currentIndex() == static_cast<int>(Page::Borrow)) {
        refreshSlotsFromDatabase();
    }
}

void MainWindow::refreshSlotsFromDatabase()
{
    if (m_currentStationId == 0) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择站点"));
        return;
    }
    
    if (!DatabaseManager::init()) {
        QMessageBox::critical(this, tr("数据库错误"), tr("无法连接到数据库"));
        return;
    }
    
    // 从数据库查询该站点的所有雨具
    auto gears = DatabaseManager::fetchGearsByStation(m_currentStationId);
    
    // 创建槽位映射：slot_id -> gear
    QMap<int, DatabaseManager::GearRecord> slotMap;
    for (const auto &gear : gears) {
        if (gear.slotId >= 1 && gear.slotId <= 12) {
            slotMap[gear.slotId] = gear;
        }
    }
    
    // 固定雨具类型分配：1-4普通塑料伞，5-8高质量抗风伞，9-10专用遮阳伞，11-12雨衣
    static const QMap<int, std::pair<GearType, QString>> slotTypeMap = {
        {1, {GearType::StandardPlastic, tr("普通塑料伞")}},
        {2, {GearType::StandardPlastic, tr("普通塑料伞")}},
        {3, {GearType::StandardPlastic, tr("普通塑料伞")}},
        {4, {GearType::StandardPlastic, tr("普通塑料伞")}},
        {5, {GearType::PremiumWindproof, tr("高质量抗风伞")}},
        {6, {GearType::PremiumWindproof, tr("高质量抗风伞")}},
        {7, {GearType::PremiumWindproof, tr("高质量抗风伞")}},
        {8, {GearType::PremiumWindproof, tr("高质量抗风伞")}},
        {9, {GearType::Sunshade, tr("专用遮阳伞")}},
        {10, {GearType::Sunshade, tr("专用遮阳伞")}},
        {11, {GearType::Raincoat, tr("雨衣")}},
        {12, {GearType::Raincoat, tr("雨衣")}}
    };
    
    // 使用RainGearFactory创建雨具对象并设置到槽位
    for (int i = 0; i < m_slots.size() && i < 12; ++i) {
        int slotId = i + 1; // 槽位ID从1开始
        auto *slot = m_slots[i];
        
        // 获取该槽位的固定类型
        auto typeInfo = slotTypeMap.value(slotId);
        GearType expectedType = typeInfo.first;
        QString typeName = typeInfo.second;
        
        // 创建对应类型的雨具对象（用于获取图标）
        QString gearId = slotMap.contains(slotId) ? slotMap[slotId].gearId : QStringLiteral("PLACEHOLDER_%1").arg(slotId);
        auto rainGear = RainGearFactory::create_raingear(expectedType, gearId);
        
        if (rainGear) {
            // 设置图标（直接加载，不缩放）
            QString iconPath = rainGear->get_iconpath();
            QPixmap icon(iconPath);
            if (icon.isNull()) {
                qWarning() << "[MainWindow] 无法加载图标:" << iconPath;
            } else {
                // 确保图标清晰，不进行任何缩放处理
                qDebug() << "[MainWindow] 加载图标:" << iconPath << "原始尺寸:" << icon.size();
            }
            QString label = QStringLiteral("#%1").arg(slotId);
            slot->setIcon(icon, label);
            slot->setGearTypeName(typeName); // 设置雨具类型名称
            
            // 根据数据库状态设置颜色
            if (slotMap.contains(slotId)) {
                const auto &gear = slotMap[slotId];
                qDebug() << "[MainWindow] 槽位" << slotId << "有雨具" << gear.gearId << "状态:" << gear.status;
                if (gear.status == 1) {
                    // 可借状态 - 绿色
                    slot->setState(SlotItem::State::Available);
                    qDebug() << "[MainWindow] 槽位" << slotId << "设置为Available(绿色)";
                } else if (gear.status == 2) {
                    // 已借出 - 灰色（空槽）
                    slot->setState(SlotItem::State::Empty);
                    qDebug() << "[MainWindow] 槽位" << slotId << "设置为Empty(灰色) - 已借出";
                } else if (gear.status == 3) {
                    // 损坏 - 红色
                    slot->setState(SlotItem::State::Maintenance);
                    qDebug() << "[MainWindow] 槽位" << slotId << "设置为Maintenance(红色) - 损坏";
                }
            } else {
                // 该槽位没有雨具 - 灰色（空槽）
                slot->setState(SlotItem::State::Empty);
                qDebug() << "[MainWindow] 槽位" << slotId << "设置为Empty(灰色) - 无雨具";
            }
        } else {
            // 无法创建雨具对象
            qWarning() << "[MainWindow] 无法创建雨具对象，slotId:" << slotId;
            slot->setState(SlotItem::State::Empty);
            slot->setIcon(QPixmap(), QStringLiteral("#%1").arg(slotId));
            slot->setGearTypeName(typeName);
        }
        
        // 确保样式已应用并立即刷新
        slot->setEnabled(true);
        slot->repaint();
        slot->update();
    }
    
    qDebug() << "[MainWindow] 已刷新站点" << m_currentStationId << "的槽位状态，共" << gears.size() << "个雨具";
}

void MainWindow::handleBorrowGear(int slotId, int slotIndex)
{
    if (!m_currentUser || m_currentStationId == 0) {
        QMessageBox::warning(this, tr("错误"), tr("请先登录并选择站点"));
        return;
    }
    
    // 查询该槽位的雨具
    auto gears = DatabaseManager::fetchGearsByStation(m_currentStationId);
    QString gearId;
    int typeId = 0;
    
    for (const auto &gear : gears) {
        if (gear.slotId == slotId && gear.status == 1) {
            gearId = gear.gearId;
            typeId = gear.typeId;
            break;
        }
    }
    
    if (gearId.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("该槽位没有可借的雨具"));
        return;
    }
    
    // 检查用户是否已有借出的雨具
    auto currentBorrow = DatabaseManager::fetchUserCurrentBorrow(m_currentUser->get_id());
    if (currentBorrow) {
        QMessageBox::warning(this, tr("提示"), tr("您还有未归还的雨具，请先归还后再借。"));
        return;
    }
    
    // 计算押金
    double deposit = 0.0;
    switch (typeId) {
        case 1: deposit = 10.0; break;  // 普通塑料伞
        case 2: deposit = 20.0; break;  // 高质量抗风伞
        case 3: deposit = 15.0; break; // 遮阳伞
        case 4: deposit = 25.0; break; // 雨衣
        default: deposit = 20.0; break;
    }
    
    // 检查余额
    if (m_currentUser->get_credit() < deposit) {
        QMessageBox::warning(this, tr("余额不足"), 
            tr("您的余额不足，需要押金 %1 元，当前余额 %2 元").arg(deposit).arg(m_currentUser->get_credit()));
        return;
    }
    
    // 确认对话框
    int ret = QMessageBox::question(this, tr("确认借伞"), 
        tr("确认借出该雨具？\n押金：%1 元\n当前余额：%2 元").arg(deposit).arg(m_currentUser->get_credit()),
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    // 执行借伞操作
    if (!DatabaseManager::borrowGear(m_currentUser->get_id(), gearId, m_currentStationId, slotId, deposit)) {
        QMessageBox::critical(this, tr("借伞失败"), tr("借伞操作失败，请重试或联系管理员。"));
        return;
    }
    
    // 更新用户余额显示
    m_currentUser->deduct(deposit);
    updateProfileFromUser();
    
    // 刷新槽位状态（借伞后，该槽位变为空，应该显示灰色）
    refreshSlotsFromDatabase();
    
    // 强制刷新UI，确保样式立即更新
    for (auto *slot : m_slots) {
        slot->style()->unpolish(slot);
        slot->style()->polish(slot);
        slot->update();
        slot->repaint();
    }
    QApplication::processEvents(); // 处理事件循环，确保UI立即更新
    
    QMessageBox::information(this, tr("借伞成功"), tr("雨具已成功借出！\n押金：%1 元已扣除").arg(deposit));
}

void MainWindow::loadMapStations(QWidget *mapContainer)
{
    if (!mapContainer) return;
    
    if (!DatabaseManager::init()) {
        qWarning() << "[MainWindow] 数据库连接失败，无法加载站点";
        return;
    }
    
    // 从数据库查询所有站点（包含坐标信息）
    auto allStations = DatabaseManager::fetchAllStations();
    
    // 查询所有站点的库存信息
    QMap<int, int> stationInventory; // station_id -> 可借雨具数量
    for (const auto &station : allStations) {
        auto gears = DatabaseManager::fetchGearsByStation(station.stationId);
        int availableCount = 0;
        for (const auto &gear : gears) {
            if (gear.status == 1) { // 可借状态
                availableCount++;
            }
        }
        stationInventory[station.stationId] = availableCount;
    }
    
    // 从JSON配置文件读取站点描述信息（可选，用于显示更详细的站点说明）
    QMap<int, QString> stationDescriptions;
    QFile configFile("map_resources/map_config.json");
    if (configFile.open(QIODevice::ReadOnly)) {
        QByteArray data = configFile.readAll();
        configFile.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject config = doc.object();
            QJsonArray stations = config["stations"].toArray();
            for (const QJsonValue &value : stations) {
                QJsonObject stationObj = value.toObject();
                int stationId = stationObj["station_id"].toInt();
                QString description = stationObj["description"].toString();
                stationDescriptions[stationId] = description;
            }
        }
    }
    
    // 根据数据库中的坐标绘制站点
    for (const auto &station : allStations) {
        int stationId = station.stationId;
        QString name = station.name;
        double posX = station.posX;  // 从数据库读取的坐标
        double posY = station.posY;  // 从数据库读取的坐标
        QString description = stationDescriptions.value(stationId, tr("雨具借还站点"));
        
        // 创建站点按钮
        auto *stationBtn = new QPushButton(mapContainer);
        stationBtn->setFixedSize(24, 24);
        stationBtn->setCursor(Qt::PointingHandCursor);
        
        // 根据库存数量设置颜色
        int availableCount = stationInventory.value(stationId, 0);
        QString color;
        if (availableCount >= 5) {
            color = "#2ecc71"; // 绿色 - 库存充足
        } else if (availableCount >= 2) {
            color = "#f1c40f"; // 黄色 - 库存紧张
        } else {
            color = "#e74c3c"; // 红色 - 库存不足
        }
        
        stationBtn->setStyleSheet(QString(
            "QPushButton {"
            "  background-color: %1;"
            "  border: 2px solid white;"
            "  border-radius: 12px;"
            "}"
            "QPushButton:hover {"
            "  background-color: %1;"
            "  border: 3px solid #3498db;"
            "  border-radius: 12px;"
            "}"
        ).arg(color));
        
        // 设置工具提示
        stationBtn->setToolTip(QString("%1\n可借雨具：%2 把\n%3")
            .arg(name).arg(availableCount).arg(description));
        
        // 点击站点时显示详细信息
        connect(stationBtn, &QPushButton::clicked, this, [this, stationId, name, availableCount, description]() {
            QString msg = QString("<h3>%1</h3>"
                "<p><b>可借雨具数量：</b>%2 把</p>"
                "<p><b>站点说明：</b>%3</p>")
                .arg(name).arg(availableCount).arg(description);
            QMessageBox::information(this, tr("站点信息"), msg);
        });
        
        // 添加站点名称标签
        auto *nameLabel = new QLabel(name, mapContainer);
        nameLabel->setStyleSheet("font-size: 11px; font-weight: 600; color: #2c3e50; background-color: rgba(255, 255, 255, 200); padding: 2px 6px; border-radius: 3px;");
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->adjustSize();
        
        // 使用定时器延迟设置位置（确保mapContainer已经完成布局）
        QTimer::singleShot(100, this, [mapContainer, stationBtn, nameLabel, posX, posY]() {
            int containerWidth = mapContainer->width();
            int containerHeight = mapContainer->height();
            
            int x = static_cast<int>(containerWidth * posX) - 12; // 减去按钮半径
            int y = static_cast<int>(containerHeight * posY) - 12;
            
            stationBtn->move(x, y);
            nameLabel->move(x - nameLabel->width() / 2 + 12, y + 28); // 放在按钮下方
        });
    }
    
    qDebug() << "[MainWindow] 已从数据库加载" << allStations.size() << "个站点并绘制到地图";
}

void MainWindow::handleReturnGear(int slotId, int slotIndex)
{
    if (!m_currentUser || m_currentStationId == 0) {
        QMessageBox::warning(this, tr("错误"), tr("请先登录并选择站点"));
        return;
    }
    
    // 检查用户是否有借出的雨具
    auto currentBorrow = DatabaseManager::fetchUserCurrentBorrow(m_currentUser->get_id());
    if (!currentBorrow) {
        QMessageBox::warning(this, tr("提示"), tr("您当前没有借出的雨具"));
        return;
    }
    
    QString gearId = currentBorrow->gearId;
    
    // 获取借出雨具的类型
    auto borrowedGear = DatabaseManager::fetchGearById(gearId);
    if (!borrowedGear) {
        QMessageBox::warning(this, tr("错误"), tr("无法获取借出雨具的信息"));
        return;
    }
    int borrowedTypeId = borrowedGear->typeId;
    
    // 根据槽位编号确定该槽位期望的雨具类型
    // 1-4号槽位：普通塑料伞(1)，5-8号槽位：高质量抗风伞(2)，9-10号槽位：遮阳伞(3)，11-12号槽位：雨衣(4)
    int expectedTypeId = 0;
    QString expectedTypeName;
    if (slotId >= 1 && slotId <= 4) {
        expectedTypeId = 1;
        expectedTypeName = tr("普通塑料伞");
    } else if (slotId >= 5 && slotId <= 8) {
        expectedTypeId = 2;
        expectedTypeName = tr("高质量抗风伞");
    } else if (slotId >= 9 && slotId <= 10) {
        expectedTypeId = 3;
        expectedTypeName = tr("专用遮阳伞");
    } else if (slotId >= 11 && slotId <= 12) {
        expectedTypeId = 4;
        expectedTypeName = tr("雨衣");
    }
    
    // 获取借出雨具的类型名称
    QString borrowedTypeName;
    switch (borrowedTypeId) {
        case 1: borrowedTypeName = tr("普通塑料伞"); break;
        case 2: borrowedTypeName = tr("高质量抗风伞"); break;
        case 3: borrowedTypeName = tr("专用遮阳伞"); break;
        case 4: borrowedTypeName = tr("雨衣"); break;
        default: borrowedTypeName = tr("未知类型"); break;
    }
    
    // 检查雨具类型是否匹配槽位
    if (borrowedTypeId != expectedTypeId) {
        QMessageBox::warning(this, tr("类型不匹配"), 
            tr("您借的是【%1】，不能归还到【%2】的槽位。\n\n请选择 %3 号槽位归还。")
            .arg(borrowedTypeName)
            .arg(expectedTypeName)
            .arg(borrowedTypeId == 1 ? "1-4" : 
                 borrowedTypeId == 2 ? "5-8" : 
                 borrowedTypeId == 3 ? "9-10" : "11-12"));
        return;
    }
    
    // 检查该槽位是否为空（槽位为空表示可以还伞）
    auto gears = DatabaseManager::fetchGearsByStation(m_currentStationId);
    bool slotOccupied = false;
    for (const auto &gear : gears) {
        if (gear.slotId == slotId) {
            slotOccupied = true;
            qDebug() << "[MainWindow] 槽位" << slotId << "已被占用，雨具ID:" << gear.gearId << "状态:" << gear.status;
            break;
        }
    }
    
    if (slotOccupied) {
        QMessageBox::warning(this, tr("提示"), tr("该槽位已被占用，请选择其他空槽位"));
        return;
    }
    
    // 确认对话框
    int ret = QMessageBox::question(this, tr("确认还伞"), 
        tr("确认将【%1】归还到槽位 #%2？").arg(borrowedTypeName).arg(slotId),
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    // 执行还伞操作
    auto [success, cost] = DatabaseManager::returnGear(m_currentUser->get_id(), gearId, m_currentStationId, slotId);
    
    if (!success) {
        QMessageBox::critical(this, tr("还伞失败"), tr("还伞操作失败，请重试或联系管理员。"));
        return;
    }
    
    // 更新用户余额显示（押金已退还，扣除费用）
    // 注意：returnGear已经处理了余额更新，这里需要重新从数据库加载用户信息
    auto userRecord = DatabaseManager::fetchUserByIdAndName(m_currentUser->get_id(), m_currentUser->get_name());
    if (userRecord) {
        m_currentUser->recharge(userRecord->credit - m_currentUser->get_credit());
    }
    updateProfileFromUser();
    
    // 刷新槽位状态（还伞后，该槽位变为有雨具，应该显示绿色）
    refreshSlotsFromDatabase();
    
    // 强制刷新UI，确保样式立即更新
    for (auto *slot : m_slots) {
        slot->style()->unpolish(slot);
        slot->style()->polish(slot);
        slot->update();
        slot->repaint();
    }
    QApplication::processEvents(); // 处理事件循环，确保UI立即更新
    
    QString msg = tr("雨具已成功归还！");
    if (cost > 0) {
        msg += tr("\n使用费用：%1 元").arg(cost);
    } else {
        msg += tr("\n24小时内免费，已全额退还押金");
    }
    QMessageBox::information(this, tr("还伞成功"), msg);
}

