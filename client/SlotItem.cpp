#include "SlotItem.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPixmap>
#include <QApplication>
#include <QStyle>
#include <QGraphicsDropShadowEffect>

SlotItem::SlotItem(int index, QWidget *parent)
    : QWidget(parent)
    , m_index(index)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setFixedSize(72, 72);
    m_iconLabel->setScaledContents(false);

    // 数字和状态指示器的布局
    auto *labelLayout = new QHBoxLayout();
    labelLayout->setContentsMargins(0, 0, 0, 0);
    labelLayout->setSpacing(8);
    labelLayout->setAlignment(Qt::AlignCenter);

    m_label = new QLabel(QStringLiteral("#%1").arg(index + 1), this);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet("font-size: 13px; font-weight: 600; color: #1a1a2e;");

    // 状态指示器（圆形）
    m_statusIndicator = new QLabel(this);
    m_statusIndicator->setFixedSize(14, 14);
    m_statusIndicator->setStyleSheet("border-radius: 7px; background-color: #bdc3c7;");

    labelLayout->addWidget(m_label);
    labelLayout->addWidget(m_statusIndicator);

    mainLayout->addWidget(m_iconLabel, 0, Qt::AlignCenter);
    mainLayout->addLayout(labelLayout);

    setMinimumSize(140, 140);
    setCursor(Qt::PointingHandCursor);
    setState(State::Available);
}

void SlotItem::setState(State state)
{
    m_state = state;
    refreshStyle();
}

void SlotItem::setIcon(const QPixmap &pixmap, const QString &descText)
{
    if (!pixmap.isNull()) {
        // 按照标签大小等比缩放，保证完整显示且尽量清晰
        QPixmap scaled = pixmap.scaled(
            m_iconLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        m_iconLabel->setPixmap(scaled);
        m_iconLabel->show();
    } else {
        // 如果没有图标，隐藏图标标签
        m_iconLabel->clear();
        m_iconLabel->hide();
    }
    if (!descText.isEmpty()) {
        m_label->setText(descText);
    }
}

void SlotItem::setGearTypeName(const QString &typeName)
{
    QString currentText = m_label->text();
    if (currentText.startsWith("#")) {
        m_label->setText(QStringLiteral("%1\n%2").arg(currentText).arg(typeName));
    } else {
        m_label->setText(QStringLiteral("%1\n%2").arg(QStringLiteral("#%1").arg(m_index + 1)).arg(typeName));
    }
    m_label->setStyleSheet("font-size: 11px; font-weight: 600; color: #4a4a68; line-height: 1.3;");
}

void SlotItem::refreshStyle()
{
    QString indicatorColor;
    QString bgGradient;
    QString borderColor;
    
    switch (m_state) {
    case State::Available:   
        indicatorColor = "#00d68f";
        borderColor = "#00d68f";
        bgGradient = "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffffff, stop:1 #f0fff5)";
        break;
    case State::Empty:       
        indicatorColor = "#bdc3c7";
        borderColor = "#e4e9f2";
        bgGradient = "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffffff, stop:1 #f8f9fa)";
        break;
    case State::Maintenance: 
        indicatorColor = "#ff3d71";
        borderColor = "#ff3d71";
        bgGradient = "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffffff, stop:1 #fff5f5)";
        break;
    case State::Selected:    
        indicatorColor = "#667eea";
        borderColor = "#667eea";
        bgGradient = "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f8f5ff, stop:1 #f0ebff)";
        break;
    }

    // 状态指示器（圆形）
    m_statusIndicator->setStyleSheet(QStringLiteral(
        "QLabel { "
        "background-color: %1; "
        "border: none; "
        "border-radius: 7px; "
        "min-width: 14px; "
        "min-height: 14px; "
        "max-width: 14px; "
        "max-height: 14px; "
        "}").arg(indicatorColor));
    
    // 清除旧样式
    setStyleSheet("");
    QApplication::processEvents();
    
    // 设置现代化卡片样式
    setStyleSheet(QStringLiteral(
        "SlotItem { "
        "background: %1; "
        "border-radius: 16px; "
        "border: 2px solid %2; "
        "}"
        "SlotItem:hover { "
        "border-width: 3px; "
        "}").arg(bgGradient, borderColor));
    
    style()->unpolish(this);
    style()->polish(this);
    update();
    repaint();
}

void SlotItem::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    emit clicked(m_index, m_state);
}

