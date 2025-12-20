# -*- coding: utf-8 -*-
"""
CampusRain 工具函数
"""
from flask import jsonify
from functools import wraps
from flask import request


def api_response(success: bool, data=None, message: str = "", code: int = 200):
    """统一的API响应格式"""
    response = {
        "success": success,
        "message": message,
        "data": data
    }
    return jsonify(response), code


def require_json(*required_fields):
    """装饰器：检查请求JSON中是否包含必需字段"""
    def decorator(f):
        @wraps(f)
        def wrapper(*args, **kwargs):
            if not request.is_json:
                return api_response(False, message="请求必须是JSON格式", code=400)
            data = request.get_json()
            missing = [field for field in required_fields if field not in data]
            if missing:
                return api_response(False, message=f"缺少必需字段: {', '.join(missing)}", code=400)
            return f(*args, **kwargs)
        return wrapper
    return decorator


def register_error_handlers(app):
    """注册错误处理器"""
    
    @app.errorhandler(404)
    def not_found(e):
        return api_response(False, message="API不存在", code=404)
    
    @app.errorhandler(500)
    def internal_error(e):
        return api_response(False, message="服务器内部错误", code=500)

