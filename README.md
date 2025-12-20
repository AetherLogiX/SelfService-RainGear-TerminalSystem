# ☂️ Distributed Self-Service Umbrella System (分布式自助雨伞借还系统)

![C++](https://img.shields.io/badge/C++-17-blue.svg) ![Qt](https://img.shields.io/badge/Qt-5.15%2B-green.svg) ![Python](https://img.shields.io/badge/Python-Flask-yellow.svg) ![MySQL](https://img.shields.io/badge/Database-MySQL-orange.svg)

## 📖 项目简介 (Introduction)

本项目是一个基于 **C/S 架构** 的分布式自助雨伞借还终端系统。旨在解决校园/园区内突发降雨时的雨具需求。用户可以通过终端设备完成注册、充值、借伞、还伞等操作，管理员可进行设备监控与数据管理。

本项目采用 **敏捷开发** 模式，经历了从“客户端直连数据库”到“前后端分离”的架构演进。

---

## 🛠️ 技术栈 (Tech Stack)

### 🖥️ 客户端 (Client / Terminal)
* **开发语言**: C++ 
* **GUI 框架**: Qt Widgets (Qt 5/14+)
* **主要功能**: 用户交互界面、数据展示、硬件模拟（模拟借还动作）
* **通信协议**: HTTP/JSON (V2版本), TCP/IP

### ⚙️ 服务端 (Server / Backend)
* **开发语言**: Python 3.9+
* **Web 框架**: Flask
* **ORM 框架**: SQLAlchemy
* **主要功能**: 业务逻辑处理、Token 验证、并发请求处理

### 🗄️ 数据库 (Database)
* **类型**: MySQL 8.0
* **部署**: 本地或云端数据库

---

## 🚀 项目架构演进 (Architecture & Roadmap)

......

## 📂 目录结构 (Directory Structure)

```text
MyUmbrellaProject/
├── Client_Qt/          # Qt 客户端源码
│   ├── Forms/          # .ui 界面文件
│   ├── Headers/        # .h 头文件
│   ├── Sources/        # .cpp 源文件
│   └── Resources/      # 图片、图标资源
│
├── Server_Flask/       # Python 服务端源码 (V2)
│   ├── app.py          # 入口文件
│   ├── models.py       # 数据库模型 (User, Umbrella)
│   └── config.py       # 数据库配置
│
├── Doc/                # 项目文档 (ER图, 需求文档)
├── sql/                # 数据库建表脚本 (.sql)
└── README.md           # 项目说明