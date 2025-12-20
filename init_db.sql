-- RainHub 校园智能共享雨具终端系统
-- 数据库初始化脚本 (MySQL 8.0)

-- 创建数据库
CREATE DATABASE IF NOT EXISTS rainhub_db 
    CHARACTER SET utf8mb4 
    COLLATE utf8mb4_unicode_ci;

-- 使用数据库
USE rainhub_db;

-- 1用户信息表 (users)
CREATE TABLE IF NOT EXISTS users (
    user_id VARCHAR(20) NOT NULL COMMENT '学号/工号',
    password VARCHAR(64) NULL COMMENT '登录密码 (首次登录为NULL，需要激活设置密码)',
    real_name VARCHAR(20) NOT NULL COMMENT '真实姓名',
    role INT NOT NULL DEFAULT 0 COMMENT '角色标识 (0:学生, 1:教职工, 9:管理员)',
    credit DECIMAL(10, 2) NOT NULL DEFAULT 0.00 COMMENT '账户余额 (单位: 元)',
    avatar_id INT NOT NULL DEFAULT 1 COMMENT '头像资源索引 ID',
    is_active TINYINT(1) NOT NULL DEFAULT 0 COMMENT '是否已激活 (0:未激活需首次设置密码, 1:已激活)',
    PRIMARY KEY (user_id),
    INDEX idx_role (role),
    INDEX idx_credit (credit),
    INDEX idx_is_active (is_active)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='用户信息表';


-- 2站点/终端表 (station)
CREATE TABLE IF NOT EXISTS station (
    station_id INT NOT NULL AUTO_INCREMENT COMMENT '站点编号',
    name VARCHAR(50) NOT NULL COMMENT '站点名称',
    pos_x FLOAT NOT NULL COMMENT '地图相对坐标 X (0.0 - 1.0)',
    pos_y FLOAT NOT NULL COMMENT '地图相对坐标 Y (0.0 - 1.0)',
    status INT NOT NULL DEFAULT 1 COMMENT '状态 (1:在线, 0:离线)',
    unavailable_slots VARCHAR(50) NOT NULL DEFAULT '' COMMENT '故障槽位列表,逗号分隔的数据库slot_id(1-12),如"1,5,8"表示第1、5、8号槽位故障',
    PRIMARY KEY (station_id),
    INDEX idx_status (status),
    INDEX idx_position (pos_x, pos_y)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='站点/终端表';


-- 3雨具库存表 (raingear)
CREATE TABLE IF NOT EXISTS raingear (
    gear_id VARCHAR(20) NOT NULL COMMENT '雨具唯一编码 (RFID芯片号)',
    type_id INT NOT NULL COMMENT '种类 (1:普通塑料伞, 2:高质量抗风伞, 3:专用遮阳伞, 4:雨衣)',
    station_id INT NULL COMMENT '当前所在的站点ID (若借出则为NULL)',
    slot_id INT NULL COMMENT '当前所在的卡槽编号 (范围1-12，与C++代码索引一致，不使用0索引)',
    status INT NOT NULL DEFAULT 1 COMMENT '状态 (1:可借, 2:已借出, 3:损坏)',
    PRIMARY KEY (gear_id),
    INDEX idx_type (type_id),
    INDEX idx_status (status),
    INDEX idx_station (station_id),
    FOREIGN KEY (station_id) REFERENCES station(station_id) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='雨具库存表';


-- 4借还记录表 (record)
CREATE TABLE IF NOT EXISTS record (
    record_id BIGINT NOT NULL AUTO_INCREMENT COMMENT '流水号',
    user_id VARCHAR(20) NOT NULL COMMENT '借用人',
    gear_id VARCHAR(20) NOT NULL COMMENT '借用的物品',
    borrow_time DATETIME NOT NULL COMMENT '借出时间',
    return_time DATETIME NULL COMMENT '归还时间 (未还则为NULL)',
    cost DECIMAL(10, 2) NOT NULL DEFAULT 0.00 COMMENT '产生的费用',
    PRIMARY KEY (record_id),
    INDEX idx_user (user_id),
    INDEX idx_gear (gear_id),
    INDEX idx_borrow_time (borrow_time),
    INDEX idx_return_time (return_time),
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE RESTRICT ON UPDATE CASCADE,
    FOREIGN KEY (gear_id) REFERENCES raingear(gear_id) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='借还记录表';


-- 创建视图

-- 1站点库存统计视图
CREATE OR REPLACE VIEW v_station_inventory AS
SELECT 
    s.station_id,
    s.name AS station_name,
    s.status AS station_status,
    COUNT(CASE WHEN r.type_id = 1 THEN 1 END) AS plastic_umbrella_count,
    COUNT(CASE WHEN r.type_id = 2 THEN 1 END) AS windproof_umbrella_count,
    COUNT(CASE WHEN r.type_id = 3 THEN 1 END) AS sunshade_umbrella_count,
    COUNT(CASE WHEN r.type_id = 4 THEN 1 END) AS raincoat_count,
    COUNT(CASE WHEN r.status = 1 THEN 1 END) AS available_count,
    COUNT(*) AS total_count
FROM station s
LEFT JOIN raingear r ON s.station_id = r.station_id AND r.status = 1
GROUP BY s.station_id, s.name, s.status;

-- 2用户借还记录视图
CREATE OR REPLACE VIEW v_user_records AS
SELECT 
    r.record_id,
    r.user_id,
    u.real_name,
    r.gear_id,
    rg.type_id,
    r.borrow_time,
    r.return_time,
    r.cost,
    CASE 
        WHEN r.return_time IS NULL THEN '未归还'
        ELSE '已归还'
    END AS record_status
FROM record r
JOIN users u ON r.user_id = u.user_id
JOIN raingear rg ON r.gear_id = rg.gear_id
ORDER BY r.borrow_time DESC;


-- 初始化完成提示
SELECT 'Database initialization completed successfully!' AS message;