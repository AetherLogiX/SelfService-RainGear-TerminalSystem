# ☔ CampusRain

> 分布式校园智能雨具共享服务平台 | C++/Qt + Python/Flask + MySQL

[![C++17](https://img.shields.io/badge/C++-17-00599C?logo=cplusplus)](https://isocpp.org/)
[![Qt 6](https://img.shields.io/badge/Qt-6.x-41CD52?logo=qt)](https://www.qt.io/)
[![Flask](https://img.shields.io/badge/Flask-3.x-000000?logo=flask)](https://flask.palletsprojects.com/)
[![MySQL](https://img.shields.io/badge/MySQL-8.0-4479A1?logo=mysql&logoColor=white)](https://www.mysql.com/)

---

## ✨ 项目亮点

- 🏗️ **分布式架构** - 客户端与服务器通过 RESTful API 通信，支持多终端同时接入
- 🎨 **现代化 UI** - 采用 Glassmorphism 设计风格，Qt Widgets 实现流畅交互体验
- 🌐 **Web 管理后台** - Flask + Jinja2 构建的实时监控控制台
- 🔄 **实时数据同步** - 借还操作即时反映到所有终端和管理后台
- 📊 **完整业务闭环** - 用户注册、借还、计费、库存管理一体化

---

## 🛠️ 技术架构

```
┌─────────────────┐     HTTP/JSON     ┌─────────────────┐
│   Qt 客户端     │ ◄───────────────► │   Flask 服务器  │
│   (C++17)       │                    │   (Python)      │
└─────────────────┘                    └────────┬────────┘
                                                │
┌─────────────────┐                             │
│   Web 管理后台  │ ◄───────────────────────────┤
│   (Browser)     │                             │
└─────────────────┘                             ▼
                                       ┌─────────────────┐
                                       │     MySQL       │
                                       │   campusrain_db │
                                       └─────────────────┘
```

| 层级 | 技术选型 |
|------|----------|
| **客户端** | C++17, Qt 6 (Widgets), QNetworkAccessManager |
| **服务端** | Python 3, Flask, Flask-CORS, PyMySQL |
| **数据库** | MySQL 8.0, InnoDB |
| **通信** | RESTful API, JSON |

---

## 📸 功能展示

### 客户端界面
- 用户登录 / 首次激活
- 站点选择与雨具借还
- 实时槽位状态显示
- 个人中心与余额查询

### Web 管理后台
- 📊 实时统计仪表盘
- 🏢 站点库存监控
- ☂️ 雨具状态管理（标记故障/修复）
- 📋 借还记录查询

---

## 🚀 快速开始

### 1. 数据库初始化
```sql
source init_db.sql;
source data_insert.sql;
source data_insert2.0.sql;
```

### 2. 启动服务器
```bash
cd server
pip install -r requirements.txt
python run.py
```
访问 http://127.0.0.1:5000/admin/ 打开管理后台

### 3. 编译客户端
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
./bin/CampusRain.exe
```

---

## 📂 项目结构

```
CampusRain/
├── client/                 # Qt 客户端
│   ├── MainWindow.cpp      # 主界面逻辑
│   ├── SlotItem.cpp        # 雨具槽位组件
│   └── Styles.h            # UI 样式系统
├── Control/
│   ├── ApiClient.cpp       # HTTP API 封装
│   └── DatabaseManager.cpp # 数据访问层
├── server/                 # Flask 服务器
│   ├── app/
│   │   ├── routes/         # API 路由 (Blueprint)
│   │   ├── models.py       # 数据模型 & 业务逻辑
│   │   └── templates/      # 管理后台页面
│   └── run.py              # 启动入口
├── init_db.sql             # 数据库 Schema
└── CMakeLists.txt          # CMake 构建配置
```

---

## 💼 业务规则

| 雨具类型 | 槽位 | 押金 |
|----------|------|------|
| 普通塑料伞 | 1-4 | ¥10 |
| 高质量抗风伞 | 5-8 | ¥20 |
| 专用遮阳伞 | 9-10 | ¥15 |
| 雨衣 | 11-12 | ¥25 |

**计费**: 24小时内免费，超时 ¥1/12小时

---

## 📄 License

MIT License © 2025
