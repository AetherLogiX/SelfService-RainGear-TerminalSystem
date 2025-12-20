# CampusRain - Flask 服务器

校园智能共享雨具系统的后端 API 服务。

## 项目结构

```
server/
├── run.py              # 启动入口
├── run.bat             # Windows 启动脚本
├── requirements.txt    # Python 依赖
├── app/
│   ├── __init__.py     # 应用工厂
│   ├── config.py       # 配置文件
│   ├── database.py     # 数据库连接管理
│   ├── models.py       # 数据模型和业务逻辑
│   ├── utils.py        # 工具函数
│   ├── routes/         # API 路由（蓝图）
│   │   ├── user.py     # 用户相关 API
│   │   ├── station.py  # 站点相关 API
│   │   ├── gear.py     # 雨具相关 API
│   │   └── admin.py    # 管理后台 API
│   ├── templates/      # HTML 模板
│   │   ├── base.html
│   │   └── admin/
│   │       ├── dashboard.html
│   │       ├── stations.html
│   │       ├── gears.html
│   │       └── records.html
│   └── static/         # 静态文件
```

## 快速开始

### 1. 安装依赖

```bash
E:\python3.14\python.exe -m pip install -r requirements.txt
```

### 2. 配置数据库

确保 MySQL 服务正在运行，且已创建 `campusrain_db` 数据库。

数据库配置在 `app/config.py` 中：
```python
MYSQL_HOST = '127.0.0.1'
MYSQL_PORT = 3306
MYSQL_USER = 'root'
MYSQL_PASSWORD = 'root'
MYSQL_DATABASE = 'campusrain_db'
```

### 3. 启动服务器

```bash
# Windows - 双击运行
run.bat

# 或命令行
cd server
E:\python3.14\python.exe run.py
```

服务器启动后：
- **API 地址**: `http://127.0.0.1:5000/api/`
- **管理后台**: `http://127.0.0.1:5000/admin/`

## API 文档

### 用户认证

| 接口 | 方法 | 说明 |
|------|------|------|
| `/api/user/verify` | POST | 验证用户是否存在 |
| `/api/user/login` | POST | 用户登录 |
| `/api/user/activate` | POST | 激活账户（首次登录） |
| `/api/user/change_password` | POST | 修改密码 |
| `/api/user/balance` | GET | 获取用户余额 |
| `/api/user/current_borrow` | GET | 获取当前借出的雨具 |
| `/api/user/history` | GET | 获取借还历史 |

### 站点管理

| 接口 | 方法 | 说明 |
|------|------|------|
| `/api/stations` | GET | 获取所有站点（含库存） |
| `/api/station/<id>` | GET | 获取单个站点详情 |
| `/api/station/<id>/gears` | GET | 获取站点所有槽位状态 |

### 雨具管理

| 接口 | 方法 | 说明 |
|------|------|------|
| `/api/gear/<gear_id>` | GET | 获取雨具详情 |

### 借还操作

| 接口 | 方法 | 说明 |
|------|------|------|
| `/api/borrow` | POST | 借伞 |
| `/api/return` | POST | 还伞 |

### 系统信息

| 接口 | 方法 | 说明 |
|------|------|------|
| `/api/health` | GET | 健康检查 |
| `/api/config/deposit` | GET | 获取押金配置 |

## 管理后台

访问 `http://127.0.0.1:5000/admin/` 进入管理控制台。

### 功能

- **控制台**: 实时统计、当前借出、故障设备、站点库存概览
- **站点管理**: 查看各站点详细库存
- **雨具管理**: 筛选查看雨具、标记故障/修复
- **借还记录**: 查看所有借还历史

## API 请求示例

### 验证用户
```json
POST /api/user/verify
{
    "user_id": "202483290399",
    "real_name": "cqw"
}
```

### 借伞
```json
POST /api/borrow
{
    "user_id": "202483290399",
    "gear_id": "G001_005",
    "station_id": 1,
    "slot_id": 5
}
```

### 还伞
```json
POST /api/return
{
    "user_id": "202483290399",
    "gear_id": "G001_005",
    "station_id": 2,
    "slot_id": 6
}
```

## 统一响应格式

```json
{
    "success": true/false,
    "message": "提示信息",
    "data": { ... }
}
```

## 雨具类型与槽位对应

| 槽位编号 | 雨具类型 | 押金 |
|---------|---------|------|
| 1-4 | 普通塑料伞 | 10元 |
| 5-8 | 高质量抗风伞 | 20元 |
| 9-10 | 专用遮阳伞 | 15元 |
| 11-12 | 雨衣 | 25元 |

## 费用规则

- 24小时内归还：**免费**
- 超过24小时：**1元/12小时**

## 分布式架构

本系统采用分布式架构：

```
┌─────────────────┐     HTTP API      ┌─────────────────┐
│  Qt 客户端      │ ◄───────────────► │  Flask 服务器   │
│  (终端机)       │                    │  (后端服务)     │
└─────────────────┘                    └────────┬────────┘
                                                │
                                                │ MySQL
                                                ▼
                                       ┌─────────────────┐
                                       │  MySQL 数据库   │
                                       │  campusrain_db   │
                                       └─────────────────┘
```

- **客户端**：通过 HTTP API 与服务器通信，不直接访问数据库
- **服务器**：处理业务逻辑，管理数据库操作
- **好处**：可多客户端同时连接，数据实时同步
