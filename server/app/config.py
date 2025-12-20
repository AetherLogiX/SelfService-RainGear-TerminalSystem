# -*- coding: utf-8 -*-
"""
CampusRain 服务器配置文件
"""
import os


class Config:
    """基础配置"""
    SECRET_KEY = os.environ.get('SECRET_KEY') or 'campusrain-secret-key-2024'
    
    # MySQL 数据库配置
    MYSQL_HOST = os.environ.get('MYSQL_HOST') or '127.0.0.1'
    MYSQL_PORT = int(os.environ.get('MYSQL_PORT') or 3306)
    MYSQL_USER = os.environ.get('MYSQL_USER') or 'root'
    MYSQL_PASSWORD = os.environ.get('MYSQL_PASSWORD') or 'root'
    MYSQL_DATABASE = os.environ.get('MYSQL_DATABASE') or 'campusrain_db'
    
    # 雨具押金配置 (元)
    DEPOSIT = {
        1: 10.0,   # 普通塑料伞
        2: 20.0,   # 高质量抗风伞
        3: 15.0,   # 专用遮阳伞
        4: 25.0,   # 雨衣
    }
    
    # 雨具类型名称
    GEAR_TYPE_NAMES = {
        1: '普通塑料伞',
        2: '高质量抗风伞',
        3: '专用遮阳伞',
        4: '雨衣',
    }
    
    # 槽位类型映射 (slot_id -> type_id)
    SLOT_TYPE_MAP = {
        1: 1, 2: 1, 3: 1, 4: 1,      # 1-4号: 普通塑料伞
        5: 2, 6: 2, 7: 2, 8: 2,      # 5-8号: 高质量抗风伞
        9: 3, 10: 3,                  # 9-10号: 专用遮阳伞
        11: 4, 12: 4,                 # 11-12号: 雨衣
    }
    
    # 费用计算：24小时内免费，超过后 1元/12小时
    FREE_HOURS = 24
    HOURLY_RATE = 1.0 / 12


class DevelopmentConfig(Config):
    """开发环境配置"""
    DEBUG = True


class ProductionConfig(Config):
    """生产环境配置"""
    DEBUG = False


config = {
    'development': DevelopmentConfig,
    'production': ProductionConfig,
    'default': DevelopmentConfig
}

