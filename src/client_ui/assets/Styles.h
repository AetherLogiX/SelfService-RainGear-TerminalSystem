/* 
  QSS
  UI样式表，用于设置QWidget的样式，更现代化，更美观
  白色简洁主题
*/

#pragma once

#include <QString>


namespace Styles {

//配色方案 - 白色简洁主题
namespace Colors {
    //主色调-灰色系
    constexpr const char* Primary = "#4a5568";
    constexpr const char* PrimaryDark = "#2d3748";
    constexpr const char* PrimaryLight = "#718096";
    constexpr const char* GradientStart = "#ffffff"; // 渐变起点（白色）
    constexpr const char* GradientEnd = "#f7fafc";   // 渐变终点（浅灰）
    
    //强调色-清新绿
    constexpr const char* Accent = "#00d4aa";
    constexpr const char* AccentHover = "#00f5c4";
    
    //状态色
    constexpr const char* Success = "#00d68f";
    constexpr const char* Warning = "#ffaa00";
    constexpr const char* Danger = "#ff3d71";
    constexpr const char* Info = "#0095ff";
    
    // 中性色
    constexpr const char* TextPrimary = "#1a1a2e";
    constexpr const char* TextSecondary = "#4a4a68";
    constexpr const char* TextMuted = "#8f8fa3";
    constexpr const char* Background = "#f0f2f5";
    constexpr const char* CardBg = "#ffffff";
    constexpr const char* Border = "#e4e9f2";
}

//全局应用样式
inline QString globalStyle() {
    return QStringLiteral(R"(
        * {
            font-family: 'Microsoft YaHei UI', 'Segoe UI', 'PingFang SC', sans-serif;
        }
        
        QMainWindow {
            background: #ffffff;
        }
        
        QWidget#centralWidget {
            background: transparent;
        }
        
        QMessageBox {
            background-color: #ffffff;
        }
        
        QMessageBox QLabel {
            color: #1a1a2e;
            font-size: 14px;
        }
        
        QMessageBox QPushButton {
            min-width: 80px;
            padding: 8px 20px;
            background: #4a5568;
            color: white;
            border: none;
            border-radius: 6px;
            font-weight: 600;
        }
        
        QMessageBox QPushButton:hover {
            background: #2d3748;
        }
        
        QComboBox {
            padding: 10px 16px;
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 10px;
            font-size: 14px;
            color: #1a1a2e;
            min-height: 20px;
        }
        
        QComboBox:hover {
            border-color: #cbd5e0;
            background-color: #ffffff;
        }
        
        QComboBox:focus {
            border-color: #4a5568;
            background-color: #ffffff;
        }
        
        QComboBox::drop-down {
            border: none;
            padding-right: 10px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #4a5568;
            margin-right: 10px;
        }
        
        QComboBox QAbstractItemView {
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 6px;
            selection-background-color: #4a5568;
            selection-color: #ffffff;
            outline: none;
        }
        
        QComboBox QAbstractItemView::item {
            padding: 8px 12px;
            min-height: 28px;
            color: #1a1a2e;
            background-color: #ffffff;
        }
        
        QComboBox QAbstractItemView::item:hover {
            background-color: #f7fafc;
            color: #2d3748;
        }
        
        QComboBox QAbstractItemView::item:selected {
            background-color: #4a5568;
            color: #ffffff;
        }
        
        QLineEdit {
            padding: 12px 16px;
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 10px;
            font-size: 14px;
            color: #1a1a2e;
        }
        
        QLineEdit:hover {
            border-color: #cbd5e0;
        }
        
        QLineEdit:focus {
            border-color: #4a5568;
            background-color: #ffffff;
        }
        
        QLineEdit::placeholder {
            color: #8f8fa3;
        }
        
        QTextBrowser {
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 12px;
            padding: 16px;
            font-size: 14px;
            color: #1a1a2e;
        }
        
        QScrollBar:vertical {
            background: #f7fafc;
            width: 8px;
            border-radius: 4px;
            margin: 0;
        }
        
        QScrollBar::handle:vertical {
            background: #cbd5e0;
            border-radius: 4px;
            min-height: 30px;
        }
        
        QScrollBar::handle:vertical:hover {
            background: #a0aec0;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        
        QTableWidget {
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 12px;
            gridline-color: #e2e8f0;
            font-size: 13px;
        }
        
        QTableWidget::item {
            padding: 8px;
            color: #1a1a2e;
        }
        
        QTableWidget::item:selected {
            background-color: #edf2f7;
            color: #2d3748;
        }
        
        QHeaderView::section {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #f8f9fa, stop:1 #f0f2f5);
            color: #4a4a68;
            font-weight: 600;
            padding: 10px;
            border: none;
            border-bottom: 2px solid #e4e9f2;
        }
    )");
}

//页面容器样式（毛玻璃卡片）
inline QString pageContainer() {
    return QStringLiteral(R"(
        background-color: rgba(255, 255, 255, 0.92);
        border-radius: 24px;
        border: 1px solid rgba(255, 255, 255, 0.5);
    )");
}

//按钮样式
namespace Buttons {
    // 主要按钮 - 灰色系
    inline QString primary() {
        return QStringLiteral(R"(
            QPushButton {
                background: #4a5568;
                color: white;
                border: none;
                border-radius: 12px;
                padding: 14px 32px;
                font-size: 16px;
                font-weight: 600;
                min-width: 120px;
            }
            QPushButton:hover {
                background: #2d3748;
            }
            QPushButton:pressed {
                background: #1a202c;
            }
            QPushButton:disabled {
                background: #bdc3c7;
                color: #7f8c8d;
            }
        )");
    }
    
    //大型主要按钮
    inline QString primaryLarge() {
        return QStringLiteral(R"(
            QPushButton {
                background: #4a5568;
                color: white;
                border: none;
                border-radius: 16px;
                padding: 20px 48px;
                font-size: 20px;
                font-weight: 700;
                min-width: 200px;
            }
            QPushButton:hover {
                background: #2d3748;
            }
            QPushButton:pressed {
                background: #1a202c;
            }
        )");
    }
    
    //次要按钮-透明边框
    inline QString secondary() {
        return QStringLiteral(R"(
            QPushButton {
                background: #ffffff;
                color: #4a5568;
                border: 2px solid #4a5568;
                border-radius: 12px;
                padding: 12px 28px;
                font-size: 15px;
                font-weight: 600;
            }
            QPushButton:hover {
                background: #f7fafc;
                border-color: #2d3748;
                color: #2d3748;
            }
            QPushButton:pressed {
                background: #edf2f7;
            }
        )");
    }
    
    //强调按钮-青绿色
    inline QString accent() {
        return QStringLiteral(R"(
            QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #00d4aa, stop:1 #00b894);
                color: white;
                border: none;
                border-radius: 12px;
                padding: 14px 32px;
                font-size: 16px;
                font-weight: 600;
            }
            QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #00f5c4, stop:1 #00d4aa);
            }
            QPushButton:pressed {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #00b894, stop:1 #009b7d);
            }
        )");
    }
    
    //功能大按钮（借伞/还伞）
    inline QString feature() {
        return QStringLiteral(R"(
            QPushButton {
                background: #ffffff;
                color: #1a1a2e;
                border: 2px solid #e2e8f0;
                border-radius: 20px;
                padding: 24px;
                font-size: 22px;
                font-weight: 700;
                min-width: 220px;
                min-height: 100px;
            }
            QPushButton:hover {
                background: #f7fafc;
                border-color: #4a5568;
                color: #2d3748;
            }
            QPushButton:pressed {
                background: #edf2f7;
                border-color: #2d3748;
            }
        )");
    }
    
    // 底部导航按钮
    inline QString nav() {
        return QStringLiteral(R"(
            QPushButton {
                background: #ffffff;
                color: #4a4a68;
                border: 1px solid #e2e8f0;
                border-radius: 14px;
                padding: 12px 20px;
                font-size: 14px;
                font-weight: 600;
            }
            QPushButton:hover {
                background: #f7fafc;
                color: #2d3748;
                border-color: #4a5568;
            }
        )");
    }
    
    //侧边栏导航按钮（管理员后台）
    inline QString sideNav() {
        return QStringLiteral(R"(
            QPushButton {
                background: transparent;
                color: rgba(255, 255, 255, 0.8);
                border: none;
                border-radius: 12px;
                padding: 14px 20px;
                font-size: 15px;
                font-weight: 600;
                text-align: left;
            }
            QPushButton:hover {
                background: rgba(255, 255, 255, 0.15);
                color: #FFFFFF;
            }
        )");
    }
    
    //侧边栏导航按钮（选中状态）
    inline QString sideNavActive() {
        return QStringLiteral(R"(
            QPushButton {
                background: rgba(255, 255, 255, 0.25);
                color: #FFFFFF;
                border: none;
                border-radius: 12px;
                padding: 14px 20px;
                font-size: 15px;
                font-weight: 700;
                text-align: left;
            }
            QPushButton:hover {
                background: rgba(255, 255, 255, 0.3);
            }
        )");
    }
    
    //文字链接按钮
    inline QString link() {
        return QStringLiteral(R"(
            QPushButton {
                background: transparent;
                color: #4a5568;
                border: none;
                padding: 10px 16px;
                font-size: 14px;
                font-weight: 500;
                min-height: 32px;
                text-decoration: underline;
            }
            QPushButton:hover {
                color: #2d3748;
            }
        )");
    }
    
    //返回按钮
    inline QString back() {
        return QStringLiteral(R"(
            QPushButton {
                background: #ffffff;
                color: #4a4a68;
                border: 1px solid #e2e8f0;
                border-radius: 10px;
                padding: 10px 24px;
                font-size: 14px;
                font-weight: 500;
            }
            QPushButton:hover {
                background: #f7fafc;
                color: #2d3748;
                border-color: #4a5568;
            }
        )");
    }
    
    //退出登录按钮
    inline QString logout() {
        return QStringLiteral(R"(
            QPushButton {
                background: rgba(255, 61, 113, 0.1);
                color: #ff3d71;
                border: 1px solid rgba(255, 61, 113, 0.3);
                border-radius: 10px;
                padding: 8px 20px;
                font-size: 13px;
                font-weight: 600;
            }
            QPushButton:hover {
                background: #ff3d71;
                color: white;
                border-color: #ff3d71;
            }
        )");
    }
    
    //危险操作按钮
    inline QString danger() {
        return QStringLiteral(R"(
            QPushButton {
                background: #ff3d71;
                color: white;
                border: none;
                border-radius: 10px;
                padding: 10px 24px;
                font-size: 14px;
                font-weight: 600;
            }
            QPushButton:hover {
                background: #e63950;
            }
        )");
    }
}

//标签样式 
namespace Labels {
    //大标题
    inline QString title() {
        return QStringLiteral(R"(
            font-size: 36px;
            font-weight: 800;
            color: #1a1a2e;
            letter-spacing: -0.5px;
        )");
    }
    
    //页面标题
    inline QString pageTitle() {
        return QStringLiteral(R"(
            font-size: 26px;
            font-weight: 700;
            color: #1a1a2e;
        )");
    }
    
    //副标题
    inline QString subtitle() {
        return QStringLiteral(R"(
            font-size: 16px;
            font-weight: 400;
            color: #4a4a68;
        )");
    }
    
    //小提示文字（无背景）
    inline QString hint() {
        return QStringLiteral(R"(
            font-size: 14px;
            color: #8f8fa3;
            background: transparent;
        )");
    }
    
    //表单标签
    inline QString formLabel() {
        return QStringLiteral(R"(
            font-size: 14px;
            font-weight: 600;
            color: #4a4a68;
        )");
    }
    
    //余额显示（无背景）
    inline QString balance() {
        return QStringLiteral(R"(
            font-size: 32px;
            font-weight: 700;
            color: #00d68f;
            background: transparent;
        )");
    }
    
    //信息标签（无背景）
    inline QString info() {
        return QStringLiteral(R"(
            font-size: 18px;
            font-weight: 600;
            color: #1a1a2e;
            background: transparent;
            padding: 4px 0;
        )");
    }
    
    //欢迎页图标
    inline QString welcomeIcon() {
        return QStringLiteral(R"(
            font-size: 72px;
            color: #4a5568;
        )");
    }
    
    //侧边栏标题（管理员后台）
    inline QString sidebarTitle() {
        return QStringLiteral(R"(
            font-size: 20px;
            font-weight: 700;
            color: #FFFFFF;
            background: transparent;
        )");
    }
    
    //统计数字
    inline QString statNumber() {
        return QStringLiteral(R"(
            font-size: 28px;
            font-weight: 700;
            color: #4a5568;
            background: transparent;
        )");
    }
    
    //统计标签
    inline QString statLabel() {
        return QStringLiteral(R"(
            font-size: 13px;
            font-weight: 500;
            color: #8f8fa3;
            background: transparent;
        )");
    }
}

//槽位卡片样式
namespace SlotCard {
    inline QString available() {
        return QStringLiteral(R"(
            SlotItem {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #ffffff, stop:1 #f8fff8);
                border: 2px solid #00d68f;
                border-radius: 16px;
            }
            SlotItem:hover {
                border-color: #00f5a0;
                background: #f0fff5;
            }
        )");
    }
    
    inline QString empty() {
        return QStringLiteral(R"(
            SlotItem {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #ffffff, stop:1 #f5f5f5);
                border: 2px solid #e4e9f2;
                border-radius: 16px;
            }
            SlotItem:hover {
                border-color: #bdc3c7;
                background: #fafafa;
            }
        )");
    }
    
    inline QString maintenance() {
        return QStringLiteral(R"(
            SlotItem {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #ffffff, stop:1 #fff5f5);
                border: 2px solid #ff3d71;
                border-radius: 16px;
            }
        )");
    }
    
    inline QString selected() {
        return QStringLiteral(R"(
            SlotItem {
                background: #ffffff;
                border: 3px solid #4a5568;
                border-radius: 16px;
            }
        )");
    }
}

//地图容器样式 
inline QString mapContainer() {
    return QStringLiteral(R"(
        background: #ffffff;
        border: 2px solid #e2e8f0;
        border-radius: 16px;
    )");
}

//管理员后台侧边栏样式
inline QString adminSidebar() {
    return QStringLiteral(R"(
        background: #2d3748;
        border-radius: 0px;
    )");
}

//统计卡片样式
inline QString statCard() {
    return QStringLiteral(R"(
        background-color: #ffffff;
        border-radius: 16px;
        border: 1px solid #e2e8f0;
    )");
}

}
