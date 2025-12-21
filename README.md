# ☂️ RainHub - 校园智能共享雨具系统

![C++](https://img.shields.io/badge/C++-17-blue.svg) ![Qt](https://img.shields.io/badge/Qt-6.5%2B-green.svg) ![MySQL](https://img.shields.io/badge/Database-MySQL-orange.svg) ![CMake](https://img.shields.io/badge/Build-CMake-purple.svg)

## 📖 项目简介

**RainHub** 是一个基于 **C++** 与 **Qt** 框架开发的校园智能共享雨具终端系统。系统采用**双客户端架构**，通过共享 MySQL 数据库实现数据同步，为校园师生提供便捷的雨具借还服务。

### 核心特性

- 🎯 **双客户端设计**：用户终端客户端 + 管理员后台客户端
- 🔄 **实时数据同步**：基于 MySQL 数据库的共享存储架构
- 🎨 **现代化 UI**：基于 Qt Widgets 的触控友好界面
- 🗺️ **地理信息可视化**：校园地图展示，实时库存状态
- 🔐 **多角色支持**：学生、教职工、管理员三种身份
- 📊 **完整业务闭环**：借还、计费、库存管理一体化

---

## 🏗️ 系统架构

### 架构模式

本系统采用**双客户端直连数据库**架构，不依赖独立的服务端程序：

```
┌─────────────────┐         ┌─────────────────┐
│   用户客户端    │         │   管理员后台     │
│  (RainHub.exe)  │         │(RainHub_Admin.exe)│
└────────┬────────┘         └────────┬────────┘
         │                           │
         │      ┌──────────────┐     │
         └──────┤  MySQL 数据库  ├─────┘
                └──────────────┘
```

**架构特点**：
- ✅ **简化部署**：无需独立服务端，降低系统复杂度
- ✅ **数据一致性**：通过数据库事务保证并发安全
- ✅ **易于扩展**：可轻松添加更多客户端实例
- ⚠️ **注意事项**：需要确保数据库连接配置正确，建议使用连接池优化性能

### 客户端功能划分

#### 1. 用户终端客户端 (`RainHub.exe`)
- 用户登录与身份验证
- 雨具借还操作
- 校园地图查看
- 个人信息管理
- 使用说明查看

#### 2. 管理员后台客户端 (`RainHub_Admin.exe`)
- 系统数据监控（仪表盘）
- 雨具库存管理
- 用户账户管理
- 借还订单管理
- 站点状态监控

---

## 🛠️ 技术栈

### 开发环境
- **编程语言**: C++ 17
- **GUI 框架**: Qt 6.5+ / Qt 5.15+ (Qt Widgets)
- **数据库**: MySQL 8.0
- **构建系统**: CMake 3.16+
- **编译器**: MinGW-w64 (Windows) / GCC (Linux)

### 核心依赖
- **Qt 模块**:
  - `QtCore` - 核心功能
  - `QtWidgets` - GUI 组件
  - `QtSql` - 数据库访问
  - `QtNetwork` - 网络通信（预留）

### 数据库驱动
- MySQL 驱动：`qsqlmysql.dll`
- 依赖库：`libmysql.dll`, `libssl-3-x64.dll`, `libcrypto-3-x64.dll`

---

## 📂 项目结构

```
Rainhub/
├── client/                 # 用户终端客户端
│   ├── main.cpp           # 客户端入口
│   ├── MainWindow.h/cpp   # 主窗口
│   ├── SlotItem.h/cpp     # 雨具槽位控件
│   └── Styles.h           # 样式定义
│
├── admin/                  # 管理员后台
│   ├── main.cpp           # 管理端入口
│   └── AdminMainWindow.h/cpp  # 管理主窗口
│
├── Model/                  # 数据模型层
│   ├── User.h/cpp         # 用户类
│   ├── RainGear.hpp       # 雨具基类
│   ├── RainGear_subclasses.hpp  # 雨具子类
│   ├── RainGearFactory.h  # 工厂模式
│   ├── Stationlocal.h/cpp  # 站点类
│   ├── StationUtils.h/cpp # 站点工具类
│   └── GlobalEnum.hpp     # 全局枚举定义
│
├── Control/                # 控制层
│   ├── DatabaseManager.h/cpp  # 数据库管理器
│   └── DBHelper.h         # 数据库连接助手
│
├── rgear_icons/            # 雨具图标资源
│   ├── icons.qrc
│   └── *.png
│
├── map_resources/          # 地图资源
│   ├── map.qrc
│   └── map_config.json
│
├── dll/                    # Windows 依赖库
│   ├── libmysql.dll
│   ├── libssl-3-x64.dll
│   ├── libcrypto-3-x64.dll
│   └── sqldrivers/
│       └── qsqlmysql.dll
│
├── init_db.sql             # 数据库初始化脚本
├── data_insert.sql         # 测试数据脚本
├── CMakeLists.txt          # CMake 构建配置
└── README.md               # 项目说明
```

---

## 🚀 快速开始

### 环境要求

1. **Qt 开发环境**
   - Qt 6.5+ 或 Qt 5.15+
   - Qt Creator（推荐）或 Visual Studio
   - MinGW-w64 编译器（Windows）

2. **MySQL 数据库**
   - MySQL 8.0 Community Server
   - 创建数据库并执行初始化脚本

3. **CMake**
   - CMake 3.16 或更高版本

### 编译步骤

1. **配置数据库**
   ```sql
   -- 执行初始化脚本
   mysql -u root -p < init_db.sql
   ```

2. **配置 CMake**
   ```bash
   # 设置 Qt 路径（如果未自动检测）
   cmake -DCMAKE_PREFIX_PATH="E:/Qt/6.9.3/mingw_64" ..
   ```

3. **编译项目**
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build . --config Release
   ```

4. **运行程序**
   - 用户客户端：`build/bin/RainHub.exe`
   - 管理员后台：`build/bin/RainHub_Admin.exe`

### 数据库配置

在 `Control/DatabaseManager.cpp` 中配置数据库连接信息：

```cpp
// 修改数据库连接参数
db.setHostName("127.0.0.1");
db.setPort(3306);
db.setDatabaseName("rainhub_db");
db.setUserName("root");
db.setPassword("your_password");
```

---

## 🎯 核心功能

### 用户端功能

- ✅ **身份验证**：支持学生/教职工登录，首次登录密码设置
- ✅ **雨具借还**：可视化槽位选择，实时库存显示
- ✅ **地图导航**：校园地图展示，站点位置与库存状态
- ✅ **个人信息**：账户余额查询，密码修改
- ✅ **使用说明**：服务协议与操作指南

### 管理端功能

- ✅ **数据监控**：实时统计在线设备、在借雨具、故障数量
- ✅ **雨具管理**：按站点/槽位查看，状态更新
- ✅ **用户管理**：用户信息查询，余额管理
- ✅ **订单管理**：借还记录查询，费用统计

---

## 🗄️ 数据库设计

### 核心数据表

1. **users** - 用户信息表
   - 存储学号/工号、密码、姓名、角色、余额等

2. **station** - 站点/终端表
   - 存储站点名称、地图坐标、在线状态、故障槽位

3. **raingear** - 雨具库存表
   - 存储雨具ID、类型、所在站点、槽位、状态

4. **record** - 借还记录表
   - 存储借还流水、时间、费用等

详细表结构请参考 `init_db.sql`。

---

## 🎨 设计模式

### 面向对象设计

- **继承与多态**：`RainGear` 基类 + 多种雨具子类（普通伞、抗风伞、雨衣等）
- **工厂模式**：`RainGearFactory` 根据类型创建对应雨具对象
- **MVC 分层**：Model（数据模型）、View（界面）、Control（业务逻辑）

### 数据访问

- **DAO 模式**：`DatabaseManager` 封装数据库操作
- **连接管理**：线程安全的数据库连接池

---

## 📝 开发说明

### 编码规范

- 代码标识符使用英文，避免中文乱码
- 界面显示文本通过工具类映射为中文（如 `StationUtils`）
- 统一使用 UTF-8 编码

### 构建输出

- 客户端：`RainHub.exe`
- 管理端：`RainHub_Admin.exe`
- 所有依赖 DLL 自动复制到 `build/bin/` 目录

---

## 🔧 常见问题

### 数据库连接失败

- 检查 MySQL 服务是否启动
- 确认数据库连接参数（主机、端口、用户名、密码）
- 检查 MySQL 驱动 DLL 是否正确部署

### Qt DLL 缺失

- 确保 Qt 环境变量配置正确
- 检查 `build/bin/` 目录下是否包含必要的 Qt DLL
- 使用 `windeployqt` 工具自动部署依赖（可选）

### 中文显示乱码

- 确保源码文件使用 UTF-8 编码
- 检查数据库字符集是否为 `utf8mb4`
- 使用 `StationUtils` 等工具类进行中文映射

---

## 📄 许可证

本项目为课程设计项目，仅供学习交流使用。

---

## 👥 贡献

欢迎提交 Issue 和 Pull Request！

---

## 📧 联系方式

如有问题或建议，请通过 Issue 反馈。

---

**RainHub** - 让雨天出行更便捷 🌧️
