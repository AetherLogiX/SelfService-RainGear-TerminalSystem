#include "MainWindow.h"
#include "SlotItem.h"

#include <QApplication>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    switchPage(Page::Welcome);
    setWindowTitle("RainHub Client");
    resize(900, 700);
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
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    auto *title = new QLabel(tr("NUIST智能自助雨具系统"), page);
    title->setStyleSheet("font-size:28px; font-weight:700;");
    auto *subtitle = new QLabel(tr("欢迎您的使用"), page);
    subtitle->setStyleSheet("font-size:18px; color:#555;");

    auto *btnStart = new QPushButton(tr("开始使用"), page);
    btnStart->setFixedWidth(180);
    btnStart->setStyleSheet("padding:12px 18px; font-size:16px;");
    connect(btnStart, &QPushButton::clicked, this, [this] {
        switchPage(Page::CardRead);
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(subtitle, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(btnStart, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createCardReadPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    auto *iconLabel = new QLabel(page);
    iconLabel->setText("📱");
    iconLabel->setStyleSheet("font-size:80px;");
    iconLabel->setAlignment(Qt::AlignCenter);

    auto *tip = new QLabel(tr("请将您的一卡通放置在刷卡处"), page);
    tip->setStyleSheet("font-size:24px; font-weight:600; color:#2c3e50;");
    tip->setAlignment(Qt::AlignCenter);

    auto *subtip = new QLabel(tr("系统将自动识别您的学号和姓名"), page);
    subtip->setStyleSheet("font-size:16px; color:#7f8c8d;");
    subtip->setAlignment(Qt::AlignCenter);

    auto *btnConfirm = new QPushButton(tr("确定"), page);
    btnConfirm->setFixedSize(200, 50);
    btnConfirm->setStyleSheet("font-size:18px; padding:10px;");
    connect(btnConfirm, &QPushButton::clicked, this, [this] {
        switchPage(Page::UserInput);
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Welcome);
    });

    layout->addStretch();
    layout->addWidget(iconLabel, 0, Qt::AlignCenter);
    layout->addWidget(tip, 0, Qt::AlignCenter);
    layout->addWidget(subtip, 0, Qt::AlignCenter);
    layout->addSpacing(30);
    layout->addWidget(btnConfirm, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    layout->addStretch();
    return page;
}

QWidget* MainWindow::createUserInputPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    auto *title = new QLabel(tr("请输入您的信息"), page);
    title->setStyleSheet("font-size:22px; font-weight:700;");
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(tr("（软件模拟：请手动输入学号/工号和姓名）"), page);
    subtitle->setStyleSheet("font-size:14px; color:#7f8c8d;");
    subtitle->setAlignment(Qt::AlignCenter);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setVerticalSpacing(16);
    
    // 使用局部变量，避免与Login页面的输入框冲突
    auto *inputUser = new QLineEdit(page);
    auto *inputName = new QLineEdit(page);
    inputUser->setPlaceholderText(tr("请输入学号/工号"));
    inputName->setPlaceholderText(tr("请输入姓名"));
    inputUser->setFixedWidth(300);
    inputName->setFixedWidth(300);
    
    form->addRow(tr("学号/工号："), inputUser);
    form->addRow(tr("姓名："), inputName);

    auto *btnSubmit = new QPushButton(tr("提交"), page);
    btnSubmit->setFixedWidth(160);
    btnSubmit->setStyleSheet("font-size:16px; padding:10px;");
    connect(btnSubmit, &QPushButton::clicked, this, [this, inputUser, inputName] {
        // 直接从输入框获取值，避免指针混乱
        const QString userId = inputUser->text().trimmed();
        const QString realName = inputName->text().trimmed();

        if (userId.isEmpty() || realName.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("请输入学号/工号和姓名"));
            return;
        }

        if (!DatabaseManager::init()) {
            QMessageBox::critical(this, tr("数据库错误"), tr("无法连接到本地 MySQL，请检查服务是否已启动。"));
            return;
        }

        qDebug() << "[UserInput] 查询用户:" << userId << realName;

        // 查询用户是否存在
        auto record = DatabaseManager::fetchUserByIdAndName(userId, realName);
        if (!record) {
            QMessageBox::warning(this, tr("用户不存在"), tr("未找到该学号/工号和姓名对应的用户，请检查输入。"));
            return;
        }

        qDebug() << "[UserInput] 找到用户, is_active:" << record->isActive;

        // 保存临时用户信息
        m_tempUserId = userId;
        m_tempUserName = realName;
        m_currentRole = (record->role == 1) ? Role::Staff : Role::Student;

        // 根据is_active判断是否首次登录
        if (!record->isActive) {
            // 首次登录，跳转到设置密码页面
            qDebug() << "[UserInput] 首次登录，跳转到设置密码页面";
            switchPage(Page::FirstLogin);
        } else {
            // 非首次登录，跳转到密码登录页面
            qDebug() << "[UserInput] 非首次登录，跳转到密码登录页面";
            switchPage(Page::Login);
        }
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::CardRead);
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(subtitle, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addLayout(form);
    layout->addWidget(btnSubmit, 0, Qt::AlignCenter);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createFirstLoginPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    auto *title = new QLabel(tr("首次登录 - 设置密码"), page);
    title->setStyleSheet("font-size:22px; font-weight:700;");
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(tr("欢迎！请设置您的登录密码"), page);
    subtitle->setStyleSheet("font-size:14px; color:#7f8c8d;");
    subtitle->setAlignment(Qt::AlignCenter);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setVerticalSpacing(16);
    
    m_inputNewPass = new QLineEdit(page);
    m_inputConfirmPass = new QLineEdit(page);
    m_inputNewPass->setPlaceholderText(tr("请输入新密码"));
    m_inputConfirmPass->setPlaceholderText(tr("请再次输入新密码"));
    m_inputNewPass->setEchoMode(QLineEdit::Password);
    m_inputConfirmPass->setEchoMode(QLineEdit::Password);
    m_inputNewPass->setFixedWidth(300);
    m_inputConfirmPass->setFixedWidth(300);
    
    form->addRow(tr("新密码："), m_inputNewPass);
    form->addRow(tr("确认密码："), m_inputConfirmPass);

    auto *btnSubmit = new QPushButton(tr("完成注册"), page);
    btnSubmit->setFixedWidth(160);
    btnSubmit->setStyleSheet("font-size:16px; padding:10px;");
    connect(btnSubmit, &QPushButton::clicked, this, [this] {
        if (performFirstLogin()) {
            switchPage(Page::Dashboard);
        }
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::UserInput);
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(subtitle, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addLayout(form);
    layout->addWidget(btnSubmit, 0, Qt::AlignCenter);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createLoginPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(16);

    m_loginRoleLabel = new QLabel(tr("请选择身份"), page);
    m_loginRoleLabel->setStyleSheet("font-size:18px; font-weight:600;");

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setVerticalSpacing(12);
    m_inputUser = new QLineEdit(page);
    m_inputName = new QLineEdit(page);
    m_inputPass = new QLineEdit(page);
    m_inputPass->setEchoMode(QLineEdit::Password);
    m_inputUser->setPlaceholderText(tr("请输入学号/工号"));
    m_inputName->setPlaceholderText(tr("请输入姓名（与数据库一致）"));
    m_inputPass->setPlaceholderText(tr("请输入密码"));
    form->addRow(tr("账号："), m_inputUser);
    form->addRow(tr("姓名："), m_inputName);
    form->addRow(tr("密码："), m_inputPass);

    auto *btnLogin = new QPushButton(tr("登录"), page);
    btnLogin->setFixedWidth(160);
    connect(btnLogin, &QPushButton::clicked, this, [this] {
        if (performLogin()) {
            switchPage(Page::Dashboard);
        }
    });

    auto *btnReset = new QPushButton(tr("修改密码"), page);
    btnReset->setFlat(true);
    connect(btnReset, &QPushButton::clicked, this, [this] {
        switchPage(Page::ResetPwd);
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::UserInput);
    });

    layout->addWidget(m_loginRoleLabel, 0, Qt::AlignCenter);
    layout->addLayout(form);
    layout->addWidget(btnLogin, 0, Qt::AlignCenter);
    layout->addWidget(btnReset, 0, Qt::AlignCenter);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createResetPwdPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(16);

    auto *title = new QLabel(tr("重置密码"), page);
    title->setStyleSheet("font-size:20px; font-weight:700;");
    title->setAlignment(Qt::AlignCenter);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setVerticalSpacing(16);

    auto *inputOld = new QLineEdit(page);
    inputOld->setPlaceholderText(tr("请输入旧密码"));
    inputOld->setEchoMode(QLineEdit::Password);
    auto *inputNew = new QLineEdit(page);
    inputNew->setPlaceholderText(tr("请输入新密码"));
    inputNew->setEchoMode(QLineEdit::Password);
    auto *inputConfirm = new QLineEdit(page);
    inputConfirm->setPlaceholderText(tr("请再次输入新密码"));
    inputConfirm->setEchoMode(QLineEdit::Password);
    inputOld->setFixedWidth(300);
    inputNew->setFixedWidth(300);
    inputConfirm->setFixedWidth(300);

    form->addRow(tr("旧密码："), inputOld);
    form->addRow(tr("新密码："), inputNew);
    form->addRow(tr("确认密码："), inputConfirm);

    auto *btnSubmit = new QPushButton(tr("提交"), page);
    btnSubmit->setFixedWidth(140);
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

        if (!m_currentUser) {
            QMessageBox::warning(this, tr("错误"), tr("请先登录"));
            switchPage(Page::Login);
            return;
        }

        const QString userId = m_currentUser->get_id();
        if (!DatabaseManager::changeUserPassword(userId, oldPassword, newPassword)) {
            QMessageBox::warning(this, tr("修改失败"), tr("旧密码错误或修改失败，请重试。"));
            return;
        }

        QMessageBox::information(this, tr("修改成功"), tr("密码已成功修改！"));
        switchPage(Page::Dashboard);
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Login);
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addLayout(form);
    layout->addWidget(btnSubmit, 0, Qt::AlignCenter);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createDashboardPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(18);

    auto *title = new QLabel(tr("NUIST智能雨具系统"), page);
    title->setStyleSheet("font-size:22px; font-weight:700;");

    auto *btnBorrow = new QPushButton(tr("我要借伞"), page);
    auto *btnReturn = new QPushButton(tr("我要还伞"), page);
    btnBorrow->setFixedSize(200, 80);
    btnReturn->setFixedSize(200, 80);
    btnBorrow->setStyleSheet("font-size:18px;");
    btnReturn->setStyleSheet("font-size:18px;");

    connect(btnBorrow, &QPushButton::clicked, this, [this] {
        m_borrowMode = true;
        populateSlots(true);
        switchPage(Page::Borrow);
    });
    connect(btnReturn, &QPushButton::clicked, this, [this] {
        m_borrowMode = false;
        populateSlots(false);
        switchPage(Page::Borrow);
    });

    auto *btnInstruction = new QPushButton(tr("使用说明"), page);
    btnInstruction->setFlat(true);
    connect(btnInstruction, &QPushButton::clicked, this, [this] {
        switchPage(Page::Instruction);
    });

    auto *bottom = new QHBoxLayout();
    bottom->setContentsMargins(20, 0, 20, 0);
    bottom->setSpacing(20);
    auto *btnProfile = new QPushButton(tr("👤 个人中心"), page);
    btnProfile->setFixedWidth(150);
    auto *btnMap = new QPushButton(tr("🗺️ 查看地图"), page);
    btnMap->setFixedWidth(150);
    connect(btnProfile, &QPushButton::clicked, this, [this] {
        updateProfileFromUser();
        switchPage(Page::Profile);
    });
    connect(btnMap, &QPushButton::clicked, this, [this] {
        switchPage(Page::Map);
    });
    bottom->addWidget(btnProfile, 0, Qt::AlignLeft);
    bottom->addStretch();
    bottom->addWidget(btnMap, 0, Qt::AlignRight);

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(btnBorrow, 0, Qt::AlignCenter);
    layout->addWidget(btnReturn, 0, Qt::AlignCenter);
    layout->addWidget(btnInstruction, 0, Qt::AlignCenter);
    layout->addLayout(bottom);
    return page;
}

QWidget* MainWindow::createBorrowPage()
{
    auto *page = new QWidget(this);
    auto *outer = new QVBoxLayout(page);
    outer->setContentsMargins(16, 16, 16, 16);
    outer->setSpacing(12);

    auto *topBar = new QHBoxLayout();
    m_slotTitle = new QLabel(tr("借伞模式"), page);
    m_slotTitle->setStyleSheet("font-size:18px; font-weight:700;");
    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    topBar->addWidget(m_slotTitle);
    topBar->addStretch();
    topBar->addWidget(btnBack);
    outer->addLayout(topBar);

    auto *grid = new QGridLayout();
    grid->setSpacing(10);

    // Create 12 slot widgets (3x4)
    for (int i = 0; i < 12; ++i) {
        auto *slot = new SlotItem(i, page);
        connect(slot, &SlotItem::clicked, this, [this](int index, SlotItem::State state) {
            // Simple selection toggle
            for (auto *s : m_slots) {
                if (s->state() == SlotItem::State::Selected && s != m_slots[index]) {
                    s->setState(m_borrowMode ? SlotItem::State::Available : SlotItem::State::Empty);
                }
            }
            auto target = m_slots[index];
            if (state == SlotItem::State::Selected) {
                target->setState(m_borrowMode ? SlotItem::State::Available : SlotItem::State::Empty);
            } else {
                target->setState(SlotItem::State::Selected);
            }
        });
        m_slots.push_back(slot);
        grid->addWidget(slot, i / 4, i % 4);
    }

    outer->addLayout(grid);
    return page;
}

QWidget* MainWindow::createMapPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    auto *title = new QLabel(tr("地图占位（后续接入真实散点）"), page);
    title->setStyleSheet("font-size:18px; font-weight:700;");
    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    layout->addWidget(title, 0, Qt::AlignLeft);
    layout->addStretch();
    layout->addWidget(btnBack, 0, Qt::AlignRight);
    return page;
}

QWidget* MainWindow::createProfilePage()
{
    auto *page = new QWidget(this);
    auto *top = new QVBoxLayout(page);
    top->setContentsMargins(16, 16, 16, 16);
    top->setSpacing(12);

    // 内容区居中
    auto *center = new QWidget(page);
    auto *vbox = new QVBoxLayout(center);
    vbox->setAlignment(Qt::AlignCenter); // 整体垂直水平居中
    vbox->setSpacing(12);
    m_profileTitle = new QLabel(tr("个人信息"), center);
    m_profileTitle->setStyleSheet("font-size:22px; font-weight:700;");
    m_profileName = new QLabel(center);
    m_profileId = new QLabel(center);
    m_profileBalance = new QLabel(center);
    m_profileName->setStyleSheet("font-size:18px;");
    m_profileId->setStyleSheet("font-size:16px; color:#444;");
    m_profileBalance->setStyleSheet("font-size:18px; font-weight:600;");
    vbox->addWidget(m_profileTitle, 0, Qt::AlignHCenter);
    vbox->addWidget(m_profileName, 0, Qt::AlignHCenter);
    vbox->addWidget(m_profileId, 0, Qt::AlignHCenter);
    vbox->addWidget(m_profileBalance, 0, Qt::AlignHCenter);
    vbox->addSpacing(12);
    center->setLayout(vbox);
    // 将内容区整体加到顶层layout并占据较多空间以实现视觉居中
    top->addStretch(2);
    top->addWidget(center, 0, Qt::AlignCenter);
    top->addStretch(3);

    // 底部刷新和返回按钮
    auto *bottom = new QHBoxLayout();
    bottom->setContentsMargins(0,0,0,0);
    bottom->setSpacing(0);
    auto *btnRefresh = new QPushButton(tr("刷新余额"), page);
    btnRefresh->setFixedWidth(140);
    connect(btnRefresh, &QPushButton::clicked, this, [this] {
        updateProfileFromUser();
    });
    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });
    bottom->addWidget(btnRefresh,0,Qt::AlignLeft);
    bottom->addStretch();
    bottom->addWidget(btnBack,0,Qt::AlignRight);
    top->addLayout(bottom);
    updateProfileFromUser();
    return page;
}

QWidget* MainWindow::createInstructionPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    auto *title = new QLabel(tr("使用说明 / 服务协议"), page);
    title->setStyleSheet("font-size:20px; font-weight:700;");

    m_instructionViewer = new QTextBrowser(page);
    m_instructionViewer->setOpenExternalLinks(false);
    m_instructionViewer->setHtml(
        "<h2 align=\"center\">NUIST 智能雨具系统服务协议</h2>"
        "<hr>"
        "<h3>一、 服务对象</h3>"
        "<p>本系统仅面向 NUIST 在校教职工与学生开放，登录需验证校园一卡通账户。</p>"
        "<h3>二、 借还规则</h3>"
        "<p><b>1. 借出：</b>账户余额需大于 <b>20.00元</b> 方可使用。借出时系统将冻结相应金额作为押金。</p>"
        "<p><b>2. 归还：</b>请将雨具插入任意站点的空闲卡槽，听到“咔哒”上锁声并看到屏幕提示“归还成功”后方可离开。</p>"
        "<h3>三、 资费标准 (自动扣款)</h3>"
        "<ul>"
        "<li><b>普通雨伞/一次性雨衣：</b> 押金 20元。</li>"
        "<li><b>高级抗风伞/加厚雨衣：</b> 押金 50元。</li>"
        "<li><b>免费时长：</b> 借出后 <b>24小时内</b> 归还免费。</li>"
        "<li><b>超时费用：</b> 超过24小时，按 <b>1元/12小时</b> 从余额扣除，直至扣完押金。</li>"
        "</ul>"
        "<h3>四、 遗失与损坏</h3>"
        "<p>若雨具遗失或严重损坏导致无法归还，系统将<b>扣除全额押金</b>用于赔偿。</p>"
        "<h3>五、 联系我们</h3>"
        "<p>如遇设备故障或扣费异常，请点击主页左侧的<b>【联系客服】</b>按钮，或致电校园服务中心：5873-6110。</p>"
    );

    auto *btnBack = new QPushButton(tr("我已阅读"), page);
    btnBack->setFixedWidth(140);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    layout->addWidget(title);
    layout->addWidget(m_instructionViewer, 1);
    layout->addWidget(btnBack, 0, Qt::AlignRight);
    return page;
}

void MainWindow::switchPage(Page page)
{
    m_stack->setCurrentIndex(static_cast<int>(page));
    if (page == Page::Borrow && m_slotTitle) {
        m_slotTitle->setText(m_borrowMode ? tr("借伞模式") : tr("还伞模式"));
    }
}

void MainWindow::populateSlots(bool borrowMode)
{
    struct Visual { QString label; QString path; };
    static const QVector<Visual> visuals = {
        {QStringLiteral("#1 普通塑料伞"), QStringLiteral(":/rgear_icons/plastic_unbrella.png")},
        {QStringLiteral("#2 普通塑料伞"), QStringLiteral(":/rgear_icons/plastic_unbrella.png")},
        {QStringLiteral("#3 普通塑料伞"), QStringLiteral(":/rgear_icons/plastic_unbrella.png")},
        {QStringLiteral("#4 普通塑料伞"), QStringLiteral(":/rgear_icons/plastic_unbrella.png")},
        {QStringLiteral("#5 高质量抗风伞"), QStringLiteral(":/rgear_icons/highquality_unbrella.png")},
        {QStringLiteral("#6 高质量抗风伞"), QStringLiteral(":/rgear_icons/highquality_unbrella.png")},
        {QStringLiteral("#7 高质量抗风伞"), QStringLiteral(":/rgear_icons/highquality_unbrella.png")},
        {QStringLiteral("#8 高质量抗风伞"), QStringLiteral(":/rgear_icons/highquality_unbrella.png")},
        {QStringLiteral("#9 遮阳伞"), QStringLiteral(":/rgear_icons/sunshade_umbrella.png")},
        {QStringLiteral("#10 遮阳伞"), QStringLiteral(":/rgear_icons/sunshade_umbrella.png")},
        {QStringLiteral("#11 雨衣"), QStringLiteral(":/rgear_icons/raincoat.png")},
        {QStringLiteral("#12 雨衣"), QStringLiteral(":/rgear_icons/raincoat.png")}
    };

    // 绑定图标与描述到槽位
    const int count = std::min(m_slots.size(), visuals.size());
    for (int i = 0; i < count; ++i) {
        QPixmap icon(visuals[i].path);
        if (icon.isNull()) {
            qWarning() << "Icon load failed:" << visuals[i].path;
        }
        m_slots[i]->setIcon(icon, visuals[i].label);
    }

    // Mock 状态：绿可借、灰可还、红维修
    for (int i = 0; i < m_slots.size(); ++i) {
        auto *slot = m_slots[i];
        if (borrowMode) {
            if (i == 2) {
                slot->setState(SlotItem::State::Maintenance);
            } else if (i % 2 == 0) {
                slot->setState(SlotItem::State::Available);
            } else {
                slot->setState(SlotItem::State::Empty);
            }
        } else {
            if (i == 5) {
                slot->setState(SlotItem::State::Maintenance);
            } else if (i % 3 == 0) {
                slot->setState(SlotItem::State::Empty);
            } else {
                slot->setState(SlotItem::State::Available);
            }
        }
    }
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
        m_profileTitle->setText(tr("个人信息"));
        m_profileName->setText(tr("姓名：-"));
        m_profileId->setText(tr("账号：-"));
        m_profileBalance->setText(tr("账户余额：￥0.00"));
        m_profileBalance->setStyleSheet("font-size:18px; font-weight:600; color:#7f8c8d;");
        return;
    }

    const bool isStaff = m_currentUser->get_role() == 1;
    m_profileTitle->setText(tr("个人信息"));
    m_profileName->setText(tr("姓名：%1").arg(m_currentUser->get_name()));
    m_profileId->setText(isStaff ? tr("工号：%1").arg(m_currentUser->get_id())
                                 : tr("学号：%1").arg(m_currentUser->get_id()));
    m_profileBalance->setText(tr("账户余额：￥%1").arg(QString::number(m_currentUser->get_balance(), 'f', 2)));
    m_profileBalance->setStyleSheet("font-size:18px; font-weight:600; color:#2ecc71;");
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

    // 重新查询用户信息
    auto record = DatabaseManager::fetchUserByIdAndName(m_tempUserId, m_tempUserName);
    if (!record) {
        QMessageBox::critical(this, tr("错误"), tr("无法加载用户信息。"));
        return false;
    }

    // 创建用户对象
    m_currentUser = std::make_unique<User>(record->userId, record->realName, record->credit, record->role);
    updateProfileFromUser();

    QMessageBox::information(this, tr("注册成功"), tr("密码设置成功，账户已激活！"));
    return true;
}

bool MainWindow::performLogin()
{
    const QString userId = m_inputUser ? m_inputUser->text().trimmed() : QString();
    const QString realName = m_inputName ? m_inputName->text().trimmed() : QString();
    const QString password = m_inputPass ? m_inputPass->text() : QString();

    if (userId.isEmpty() || realName.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请输入账号、姓名和密码"));
        return false;
    }

    if (!DatabaseManager::init()) {
        QMessageBox::critical(this, tr("数据库错误"), tr("无法连接到本地 MySQL，请检查服务是否已启动。"));
        return false;
    }

    // 调用密码校验逻辑（明文密码比较）
    auto record = DatabaseManager::fetchUserByIdAndNameAndPassword(userId, realName, password);
    if (!record) {
        QMessageBox::warning(this, tr("登录失败"), tr("账号、姓名或密码错误，请检查输入。"));
        return false;
    }

    m_currentRole = (record->role == 1) ? Role::Staff : Role::Student;
    m_currentUser = std::make_unique<User>(record->userId, record->realName, record->credit, record->role);

    updateRoleLabel();
    updateProfileFromUser();
    QMessageBox::information(this, tr("登录成功"), tr("已从数据库加载用户信息。"));
    return true;
}

