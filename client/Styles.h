#pragma once

#include <QString>

/**
 * CampusRain 现代化样式系统
 * 采用深蓝渐变主题 + 玻璃拟态设计
 */
namespace Styles {

//配色方案 
namespace Colors {
    //主色调-深蓝渐变
    constexpr const char* Primary = "#667eea";
    constexpr const char* PrimaryDark = "#764ba2";
    constexpr const char* PrimaryLight = "#a8b4f0";
    
    //强调色
    constexpr const char* Accent = "#00d4aa";
    constexpr const char* AccentHover = "#00f5c4";
    
    //状态色
    constexpr const char* Success = "#00d68f";
    constexpr const char* Warning = "#ffaa00";
    constexpr const char* Danger = "#ff3d71";
    constexpr const char* Info = "#0095ff";
    
    //中性色
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
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #667eea, stop:1 #764ba2);
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
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #667eea, stop:1 #764ba2);
            color: white;
            border: none;
            border-radius: 6px;
            font-weight: 600;
        }
        
        QMessageBox QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #7b8ff0, stop:1 #8a5db0);
        }
        
        QComboBox {
            padding: 10px 16px;
            background-color: rgba(255, 255, 255, 0.95);
            border: 2px solid rgba(102, 126, 234, 0.3);
            border-radius: 10px;
            font-size: 14px;
            color: #1a1a2e;
            min-height: 20px;
        }
        
        QComboBox:hover {
            border-color: #667eea;
            background-color: #ffffff;
        }
        
        QComboBox:focus {
            border-color: #667eea;
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
            border-top: 6px solid #667eea;
            margin-right: 10px;
        }
        
        QComboBox QAbstractItemView {
            background-color: #ffffff;
            border: 2px solid #667eea;
            border-radius: 8px;
            padding: 6px;
            selection-background-color: #667eea;
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
            background-color: rgba(102, 126, 234, 0.15);
            color: #667eea;
        }
        
        QComboBox QAbstractItemView::item:selected {
            background-color: #667eea;
            color: #ffffff;
        }
        
        QLineEdit {
            padding: 12px 16px;
            background-color: rgba(255, 255, 255, 0.95);
            border: 2px solid rgba(102, 126, 234, 0.3);
            border-radius: 10px;
            font-size: 14px;
            color: #1a1a2e;
        }
        
        QLineEdit:hover {
            border-color: rgba(102, 126, 234, 0.6);
        }
        
        QLineEdit:focus {
            border-color: #667eea;
            background-color: #ffffff;
        }
        
        QLineEdit::placeholder {
            color: #8f8fa3;
        }
        
        QTextBrowser {
            background-color: rgba(255, 255, 255, 0.95);
            border: 2px solid rgba(102, 126, 234, 0.2);
            border-radius: 12px;
            padding: 16px;
            font-size: 14px;
            color: #1a1a2e;
        }
        
        QScrollBar:vertical {
            background: rgba(0, 0, 0, 0.05);
            width: 8px;
            border-radius: 4px;
            margin: 0;
        }
        
        QScrollBar::handle:vertical {
            background: rgba(102, 126, 234, 0.5);
            border-radius: 4px;
            min-height: 30px;
        }
        
        QScrollBar::handle:vertical:hover {
            background: rgba(102, 126, 234, 0.8);
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
    )");
}

// ============ 页面容器样式（玻璃拟态卡片）============
inline QString pageContainer() {
    return QStringLiteral(R"(
        background-color: rgba(255, 255, 255, 0.92);
        border-radius: 24px;
        border: 1px solid rgba(255, 255, 255, 0.5);
    )");
}

// ============ 按钮样式 ============
namespace Buttons {
    // 主要按钮 - 渐变色
    inline QString primary() {
        return QStringLiteral(R"(
            QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #667eea, stop:1 #764ba2);
                color: white;
                border: none;
                border-radius: 12px;
                padding: 14px 32px;
                font-size: 16px;
                font-weight: 600;
                min-width: 120px;
            }
            QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #7b8ff0, stop:1 #8a5db0);
            }
            QPushButton:pressed {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #5a6fd6, stop:1 #6a4192);
            }
            QPushButton:disabled {
                background: #bdc3c7;
                color: #7f8c8d;
            }
        )");
    }
    
    // 大型主要按钮
    inline QString primaryLarge() {
        return QStringLiteral(R"(
            QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #667eea, stop:1 #764ba2);
                color: white;
                border: none;
                border-radius: 16px;
                padding: 20px 48px;
                font-size: 20px;
                font-weight: 700;
                min-width: 200px;
            }
            QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #7b8ff0, stop:1 #8a5db0);
            }
            QPushButton:pressed {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #5a6fd6, stop:1 #6a4192);
            }
        )");
    }
    
    // 次要按钮 - 透明边框
    inline QString secondary() {
        return QStringLiteral(R"(
            QPushButton {
                background: rgba(255, 255, 255, 0.9);
                color: #667eea;
                border: 2px solid #667eea;
                border-radius: 12px;
                padding: 12px 28px;
                font-size: 15px;
                font-weight: 600;
            }
            QPushButton:hover {
                background: rgba(102, 126, 234, 0.1);
                border-color: #764ba2;
                color: #764ba2;
            }
            QPushButton:pressed {
                background: rgba(102, 126, 234, 0.2);
            }
        )");
    }
    
    // 强调按钮 - 青绿色
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
    
    // 功能大按钮（借伞/还伞）
    inline QString feature() {
        return QStringLiteral(R"(
            QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                    stop:0 rgba(255,255,255,0.95), stop:1 rgba(255,255,255,0.85));
                color: #1a1a2e;
                border: 2px solid rgba(102, 126, 234, 0.3);
                border-radius: 20px;
                padding: 24px;
                font-size: 22px;
                font-weight: 700;
                min-width: 220px;
                min-height: 100px;
            }
            QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                    stop:0 #ffffff, stop:1 #f8f9ff);
                border-color: #667eea;
                color: #667eea;
            }
            QPushButton:pressed {
                background: rgba(102, 126, 234, 0.1);
                border-color: #764ba2;
            }
        )");
    }
    
    // 底部导航按钮
    inline QString nav() {
        return QStringLiteral(R"(
            QPushButton {
                background: rgba(255, 255, 255, 0.85);
                color: #4a4a68;
                border: 1px solid rgba(102, 126, 234, 0.2);
                border-radius: 14px;
                padding: 12px 20px;
                font-size: 14px;
                font-weight: 600;
            }
            QPushButton:hover {
                background: #ffffff;
                color: #667eea;
                border-color: #667eea;
            }
        )");
    }
    
    // 文字链接按钮
    inline QString link() {
        return QStringLiteral(R"(
            QPushButton {
                background: transparent;
                color: #667eea;
                border: none;
                padding: 8px 16px;
                font-size: 14px;
                font-weight: 500;
                text-decoration: underline;
            }
            QPushButton:hover {
                color: #764ba2;
            }
        )");
    }
    
    // 返回按钮
    inline QString back() {
        return QStringLiteral(R"(
            QPushButton {
                background: rgba(255, 255, 255, 0.8);
                color: #4a4a68;
                border: 1px solid rgba(0, 0, 0, 0.1);
                border-radius: 10px;
                padding: 10px 24px;
                font-size: 14px;
                font-weight: 500;
            }
            QPushButton:hover {
                background: #ffffff;
                color: #667eea;
                border-color: #667eea;
            }
        )");
    }
    
    // 退出登录按钮
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
}

// ============ 标签样式 ============
namespace Labels {
    // 大标题
    inline QString title() {
        return QStringLiteral(R"(
            font-size: 36px;
            font-weight: 800;
            color: #1a1a2e;
            letter-spacing: -0.5px;
        )");
    }
    
    // 页面标题
    inline QString pageTitle() {
        return QStringLiteral(R"(
            font-size: 26px;
            font-weight: 700;
            color: #1a1a2e;
        )");
    }
    
    // 副标题
    inline QString subtitle() {
        return QStringLiteral(R"(
            font-size: 16px;
            font-weight: 400;
            color: #4a4a68;
        )");
    }
    
    // 小提示文字（无背景）
    inline QString hint() {
        return QStringLiteral(R"(
            font-size: 14px;
            color: #8f8fa3;
            background: transparent;
        )");
    }
    
    // 表单标签
    inline QString formLabel() {
        return QStringLiteral(R"(
            font-size: 14px;
            font-weight: 600;
            color: #4a4a68;
        )");
    }
    
    // 余额显示（无背景）
    inline QString balance() {
        return QStringLiteral(R"(
            font-size: 32px;
            font-weight: 700;
            color: #00d68f;
            background: transparent;
        )");
    }
    
    // 信息标签（无背景）
    inline QString info() {
        return QStringLiteral(R"(
            font-size: 18px;
            font-weight: 600;
            color: #1a1a2e;
            background: transparent;
            padding: 4px 0;
        )");
    }
    
    // 欢迎页图标
    inline QString welcomeIcon() {
        return QStringLiteral(R"(
            font-size: 72px;
            color: #667eea;
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
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #f8f5ff, stop:1 #f0ebff);
                border: 3px solid #667eea;
                border-radius: 16px;
            }
        )");
    }
}

// ============ 地图容器样式 ============
inline QString mapContainer() {
    return QStringLiteral(R"(
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
            stop:0 #e8f4f8, stop:0.5 #f0f7fa, stop:1 #e8f0f8);
        border: 2px solid rgba(102, 126, 234, 0.2);
        border-radius: 16px;
    )");
}

} // namespace Styles

