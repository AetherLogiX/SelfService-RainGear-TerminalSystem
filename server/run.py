# -*- coding: utf-8 -*-
"""
CampusRain 校园智能共享雨具系统 - Flask 服务器
"""
import sys
import os

# 添加当前目录到路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from app import create_app

app = create_app('development')

if __name__ == '__main__':
    print("=" * 55)
    print("  ☔ CampusRain - 校园智能共享雨具系统")
    print("  Flask API Server with Admin Console")
    print("=" * 55)
    print(f"  📦 数据库: {app.config['MYSQL_DATABASE']}")
    print(f"  🔧 运行模式: {'开发模式' if app.config['DEBUG'] else '生产模式'}")
    print("=" * 55)
    print("  🌐 API 地址: http://127.0.0.1:5000/api/")
    print("  🖥️  管理后台: http://127.0.0.1:5000/admin/")
    print("=" * 55)
    
    app.run(host='0.0.0.0', port=5000, debug=True)
