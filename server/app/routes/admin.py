# -*- coding: utf-8 -*-
"""
管理后台路由
"""
from flask import Blueprint, render_template, request, current_app
from app.utils import api_response
from app.models import StationModel, GearModel, BorrowModel, UserModel

admin_bp = Blueprint('admin', __name__)


# ============ Web页面路由 ============

@admin_bp.route('/')
def dashboard():
    """管理控制台首页"""
    return render_template('admin/dashboard.html')


@admin_bp.route('/stations')
def stations_page():
    """站点管理页面"""
    return render_template('admin/stations.html')


@admin_bp.route('/gears')
def gears_page():
    """雨具管理页面"""
    return render_template('admin/gears.html')


@admin_bp.route('/records')
def records_page():
    """借还记录页面"""
    return render_template('admin/records.html')


# ============ 管理API路由 ============

@admin_bp.route('/api/stats', methods=['GET'])
def get_stats():
    """获取统计数据"""
    inventory = StationModel.get_all_inventory()
    stations = StationModel.get_all()
    current_borrows = BorrowModel.get_all_current_borrows()
    
    return api_response(True, data={
        "total_stations": len(stations),
        "total_gears": inventory['total_gears'] or 0,
        "total_available": inventory['total_available'] or 0,
        "total_borrowed": inventory['total_borrowed'] or 0,
        "total_damaged": inventory['total_damaged'] or 0,
        "current_borrows_count": len(current_borrows)
    })


@admin_bp.route('/api/gears', methods=['GET'])
def get_all_gears():
    """获取所有雨具"""
    gears = GearModel.get_all()
    
    result = []
    for g in gears:
        status_text = {1: '可用', 2: '借出', 3: '故障'}.get(g['status'], '未知')
        result.append({
            "gear_id": g['gear_id'],
            "type_id": g['type_id'],
            "type_name": current_app.config['GEAR_TYPE_NAMES'].get(g['type_id'], '未知'),
            "station_id": g['station_id'],
            "station_name": g.get('station_name') or '已借出',
            "slot_id": g['slot_id'],
            "status": g['status'],
            "status_text": status_text
        })
    
    return api_response(True, data=result)


@admin_bp.route('/api/gear/<gear_id>/status', methods=['POST'])
def set_gear_status(gear_id):
    """设置雨具状态"""
    data = request.get_json()
    if not data or 'status' not in data:
        return api_response(False, message="缺少status参数")
    
    status = int(data['status'])
    if status not in [1, 2, 3]:
        return api_response(False, message="无效的状态值，应为1(可用)、2(借出)、3(故障)")
    
    success = GearModel.set_status(gear_id, status)
    if success:
        status_text = {1: '可用', 2: '借出', 3: '故障'}.get(status)
        return api_response(True, message=f"雨具状态已更新为：{status_text}")
    else:
        return api_response(False, message="更新失败，雨具不存在")


@admin_bp.route('/api/damaged', methods=['GET'])
def get_damaged_gears():
    """获取所有故障雨具"""
    gears = GearModel.get_damaged()
    
    result = []
    for g in gears:
        result.append({
            "gear_id": g['gear_id'],
            "type_id": g['type_id'],
            "type_name": current_app.config['GEAR_TYPE_NAMES'].get(g['type_id'], '未知'),
            "station_id": g['station_id'],
            "station_name": g.get('station_name') or '-',
            "slot_id": g['slot_id']
        })
    
    return api_response(True, data=result)


@admin_bp.route('/api/current_borrows', methods=['GET'])
def get_current_borrows():
    """获取所有当前借出"""
    borrows = BorrowModel.get_all_current_borrows()
    
    result = []
    for b in borrows:
        result.append({
            "record_id": b['record_id'],
            "user_id": b['user_id'],
            "real_name": b.get('real_name') or '-',
            "gear_id": b['gear_id'],
            "type_name": current_app.config['GEAR_TYPE_NAMES'].get(b.get('type_id'), '未知'),
            "borrow_time": b['borrow_time']
        })
    
    return api_response(True, data=result)


@admin_bp.route('/api/records', methods=['GET'])
def get_records():
    """获取借还记录"""
    limit = request.args.get('limit', 50, type=int)
    records = BorrowModel.get_recent_records(limit)
    return api_response(True, data=records)


@admin_bp.route('/api/stations', methods=['GET'])
def get_stations_detail():
    """获取站点详情（包含库存）"""
    stations = StationModel.get_all()
    
    result = []
    for s in stations:
        inventory = StationModel.get_inventory(s['station_id'])
        result.append({
            "station_id": s['station_id'],
            "name": s['name'],
            "status": s['status'],
            "inventory": {
                "plastic_umbrella": inventory['plastic_umbrella'] or 0,
                "windproof_umbrella": inventory['windproof_umbrella'] or 0,
                "sunshade_umbrella": inventory['sunshade_umbrella'] or 0,
                "raincoat": inventory['raincoat'] or 0,
                "total_available": inventory['total_available'] or 0,
                "total_borrowed": inventory.get('total_borrowed') or 0,
                "total_damaged": inventory.get('total_damaged') or 0
            }
        })
    
    return api_response(True, data=result)

