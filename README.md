# ☔ CampusRain

> 校园智能雨具共享服务平台 | C++/Qt  + MySQL

> **"让下雨天不再是校园出行的阻碍。"**

## 项目背景 

作为一名在校生，大家肯定都遇到过这样的尴尬时刻：早八阳光明媚，下课却突降暴雨，被困在教学楼寸步难行。**RainHub** 的灵感正是来源于此。

这不是一个简单的 CRUD 练习，而是一个尝试解决校园“最后一公里”避雨问题的完整解决方案。在这个项目中，我并没有依赖现成的后端框架，而是基于 **C++ Qt** 和 **MySQL** 构建了一套**双客户端架构（用户端 + 管理端）**，并手动实现了完整的**业务逻辑层（Service）**与**数据访问层（DAO）**的解耦。

## 项目结构 (Project Structure)

基于 `src` 源码目录的核心结构说明：

Plaintext

```
Rainhub/
├── sql/                    # 数据库初始化与测试脚本 (.sql)
├── src/
│   ├── admin_ui/           # 管理员后台界面逻辑
│   ├── client_ui/          # 用户终端界面逻辑
│   ├── control/            # [核心业务层] 包含 AuthService, BorrowService 等
│   ├── dao/                # [数据持久层] 封装 SQL 操作 (GearDao, UserDao 等)
│   ├── Model/              # 数据实体类 (User, RainGear)
│   └── main.cpp            # 程序入口
├── resources/              # 图标与地图资源文件
└── CMakeLists.txt          # 构建配置
```

## 核心功能

### 用户终端

- **自助借还**：可视化的雨具槽位选择，就像借共享充电宝一样简单。
- **实时地图**：查看校园内所有站点的雨具库存（数据实时同步）。
- **账户体系**：支持学生/教职工身份切换，余额管理。

### 管理员后台 (Admin Dashboard)

- **设备监控**：远程监控各站点状态，识别故障槽位。
- **数据大屏**：通过 `Admin_OrdersService` 统计每日流水与借还高峰。
- **库存调度**：通过 `Admin_GearService` 管理雨具的投放与回收。

## 技术栈

- **开发语言**: C++ 17
- **GUI 框架**: Qt 6.5 (Qt Widgets) - 使用 QSS 进行现代化 UI 美化
- **数据库**: MySQL 8.0 - 使用 `QSqlDatabase` 连接池技术
- **设计模式**:
  - **Factory Pattern**: 用于创建不同类型的雨具 (`RainGearFactory`)
  - **DAO Pattern**: 实现业务与数据的分离
  - **Singleton**: 用于数据库连接管理 (`DatabaseManager`)

## RUN_GUIDE

1. **环境准备**: 确保已安装 Qt 6.x (MinGW/MSVC) 及 MySQL 8.0。

2. **数据库配置**:

   - 在 MySQL 中创建一个名为 `rainhub_db` 的数据库。
   - 运行 `sql/init_db.sql` 初始化表结构。
   - 运行 `sql/data_insert.sql` 导入测试数据。

3. **修改配置**:

   - 打开 `src/control/DatabaseManager.cpp`，修改 `setUserName` 和 `setPassword` 为你的本地配置。

4. **编译运行**:

   Bash

   ```
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```