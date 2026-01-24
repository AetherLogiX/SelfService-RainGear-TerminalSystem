/*
    借还伞页面实现
*/
#include "BorrowPage.h"
#include "../assets/Styles.h"
#include "../components/SlotItem.h"
#include "../../control/BorrowService.h"
#include "../../control/StationService.h"
#include "../../Model/RainGearFactory.h"
#include "../../dao/RecordDao.h"
#include "../../utils/ConnectionPool.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>

BorrowPage::BorrowPage(BorrowService *borrowService, StationService *stationService, QWidget *parent)
    : QWidget(parent)
    , m_borrowService(borrowService)
    , m_stationService(stationService)
{
    setupUi();
    
    // 创建定时刷新器
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &BorrowPage::refreshSlots);
}

BorrowPage::~BorrowPage()
{
    stopAutoRefresh();
}

void BorrowPage::setupUi()
{
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(24, 24, 24, 24);
    outer->setSpacing(16);

    // 玻璃卡片容器
    auto *card = new QWidget(this);
    card->setStyleSheet(Styles::pageContainer());
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 20, 24, 20);
    cardLayout->setSpacing(16);

    // 顶部栏
    auto *topBar = new QHBoxLayout();
    m_titleLabel = new QLabel(tr("☔ 借伞模式"), card);
    m_titleLabel->setStyleSheet(Styles::Labels::pageTitle());
    
    auto *btnBack = new QPushButton(tr("返回主页"), card);
    btnBack->setStyleSheet(Styles::Buttons::back());
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, &BorrowPage::backRequested);

    topBar->addWidget(m_titleLabel);
    topBar->addStretch();
    topBar->addWidget(btnBack);
    cardLayout->addLayout(topBar);

    // 提示信息
    auto *hintLabel = new QLabel(card);
    hintLabel->setStyleSheet(Styles::Labels::hint());
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setText(tr("🟢 绿色=可借  ⚪ 灰色=空槽可还  🔴 红色=故障"));
    cardLayout->addWidget(hintLabel);

    // 槽位网格 (3x4)
    auto *grid = new QGridLayout();
    grid->setSpacing(14);

    for (int i = 0; i < 12; ++i) {
        auto *slot = new SlotItem(i, card);
        slot->setState(SlotItem::State::Empty);
        slot->setIcon(QPixmap(), QStringLiteral("#%1").arg(i + 1));
        
        connect(slot, &SlotItem::clicked, this, [this, i](int, SlotItem::State) {
            onSlotClicked(i);
        });
        
        m_slots.push_back(slot);
        grid->addWidget(slot, i / 4, i % 4);
    }

    cardLayout->addLayout(grid);
    outer->addWidget(card);
}

void BorrowPage::setContext(std::shared_ptr<User> user, int stationId, bool isBorrowMode)
{
    m_currentUser = user;
    m_currentStationId = stationId;
    m_isBorrowMode = isBorrowMode;
    
    m_titleLabel->setText(isBorrowMode ? tr("☔ 借伞模式") : tr("🔄 还伞模式"));
    refreshSlots();
}

void BorrowPage::refreshSlots()
{
    if (m_currentStationId == 0) return;
    
    auto station = m_stationService->getStationDetail(static_cast<Station>(m_currentStationId));
    if (!station) return;
    
    // 固定雨具类型分配：1-4普通塑料伞，5-8高质量抗风伞，9-10专用遮阳伞，11-12雨衣
    static const QMap<int, QPair<GearType, QString>> slotTypeMap = {
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
    
    for (int i = 0; i < m_slots.size() && i < 12; ++i) {
        int slotId = i + 1;
        auto *slot = m_slots[i];
        
        auto typeInfo = slotTypeMap.value(slotId);
        GearType expectedType = typeInfo.first;
        QString typeName = typeInfo.second;
        
        // 创建雨具对象获取图标
        QString gearId = QStringLiteral("PLACEHOLDER_%1").arg(slotId);
        auto rainGear = RainGearFactory::create_raingear(expectedType, gearId);
        
        if (rainGear) {
            QString iconPath = rainGear->get_iconpath();
            QPixmap icon(iconPath);
            slot->setIcon(icon, QStringLiteral("#%1").arg(slotId));
            slot->setGearTypeName(typeName);
        }
        
        // 根据站点库存设置状态
        if (station->is_gear_available(slotId)) {
            // 该槽位有雨具且可借
            slot->setState(SlotItem::State::Available);
        } else if (station->is_slot_broken(slotId)) {
            // 该槽位被标记为故障
            slot->setState(SlotItem::State::Maintenance);
        } else if (station->has_gear(slotId)) {
            // 该槽位有雨具但不可借（已借出或其他原因）
            slot->setState(SlotItem::State::Maintenance);
        } else {
            // 空槽（可还）
            slot->setState(SlotItem::State::Empty);
        }
        
        slot->setEnabled(true);
        slot->repaint();
    }
    
    QApplication::processEvents();
}

void BorrowPage::startAutoRefresh()
{
    if (m_refreshTimer && !m_refreshTimer->isActive()) {
        m_refreshTimer->start(3000);  // 每3秒刷新
    }
}

void BorrowPage::stopAutoRefresh()
{
    if (m_refreshTimer && m_refreshTimer->isActive()) {
        m_refreshTimer->stop();
    }
}

void BorrowPage::onSlotClicked(int slotIndex)
{
    if (!m_currentUser) {
        QMessageBox::warning(this, tr("错误"), tr("请先登录"));
        return;
    }
    
    int slotId = slotIndex + 1;
    
    if (m_isBorrowMode) {
        handleBorrow(slotId);
    } else {
        handleReturn(slotId);
    }
}

void BorrowPage::handleBorrow(int slotId)
{
    // 获取站点详情（用于UI检查）
    auto station = m_stationService->getStationDetail(static_cast<Station>(m_currentStationId));
    if (!station) {
        QMessageBox::warning(this, tr("错误"), tr("无法获取站点信息"));
        return;
    }
    
    // 检查槽位是否可借（UI层面的快速检查）
    if (!station->is_gear_available(slotId)) {
        QMessageBox::warning(this, tr("提示"), tr("该槽位没有可借的雨具"));
        return;
    }
    
    // 直接调用借伞服务，传入站点ID和槽位ID
    // Service层会负责查找雨具ID并执行借伞逻辑
    auto result = m_borrowService->borrowGear(
        m_currentUser->get_id(), 
        static_cast<Station>(m_currentStationId), 
        slotId
    );
    
    if (result.success) {
        QMessageBox::information(this, tr("借伞成功"), result.message);
        refreshSlots();
        emit operationCompleted();
    } else {
        QMessageBox::warning(this, tr("借伞失败"), result.message);
    }
}

void BorrowPage::handleReturn(int slotId)
{
    // 获取用户当前借出的雨具
    auto db = ConnectionPool::getThreadLocalConnection();
    RecordDao recordDao;
    auto recordOpt = recordDao.selectUnfinishedByUserId(db, m_currentUser->get_id());
    
    if (!recordOpt.has_value()) {
        QMessageBox::warning(this, tr("提示"), tr("您当前没有借出的雨具"));
        return;
    }
    
    QString gearId = recordOpt->get_gear_id();
    
    // 调用还伞服务
    auto result = m_borrowService->returnGear(
        m_currentUser->get_id(), 
        gearId, 
        static_cast<Station>(m_currentStationId), 
        slotId
    );
    
    if (result.success) {
        QString msg = result.message;
        if (result.cost > 0) {
            msg += tr("\n使用费用：%1 元").arg(result.cost);
        }
        QMessageBox::information(this, tr("还伞成功"), msg);
        refreshSlots();
        emit operationCompleted();
    } else {
        QMessageBox::warning(this, tr("还伞失败"), result.message);
    }
}

