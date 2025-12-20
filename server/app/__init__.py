# -*- coding: utf-8 -*-
"""
CampusRain Flask 应用工厂
"""
from flask import Flask
from flask_cors import CORS


def create_app(config_name='development'):
    """应用工厂函数"""
    app = Flask(__name__, 
                template_folder='templates',
                static_folder='static')
    
    # 加载配置
    from app.config import config
    app.config.from_object(config[config_name])
    
    # 启用CORS
    CORS(app)
    
    # 注册蓝图
    from app.routes.user import user_bp
    from app.routes.station import station_bp
    from app.routes.gear import gear_bp
    from app.routes.admin import admin_bp
    
    app.register_blueprint(user_bp, url_prefix='/api/user')
    app.register_blueprint(station_bp, url_prefix='/api')
    app.register_blueprint(gear_bp, url_prefix='/api')
    app.register_blueprint(admin_bp, url_prefix='/admin')
    
    # 注册错误处理
    from app.utils import register_error_handlers
    register_error_handlers(app)
    
    return app

