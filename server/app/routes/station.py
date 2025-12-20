# -*- coding: utf-8 -*-
"""
站点相关API路由
"""
from flask import Blueprint
from app.utils import api_response
from app.models import StationModel

station_bp = Blueprint('station', __name__)


@station_bp.route('/stations', methods=['GET'])
def get_all_stations():
    """获取所有站点列表"""
    stations = StationModel.get_all()
    
    result = []
    for s in stations:
        inventory = StationModel.get_inventory(s['station_id'])
        result.append({
            "station_id": s['station_id'],
            "name": s['name'],
            "pos_x": float(s['pos_x']) if s['pos_x'] else 0,
            "pos_y": float(s['pos_y']) if s['pos_y'] else 0,
            "status": s['status'],
            "unavailable_slots": s.get('unavailable_slots', ''),
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


@station_bp.route('/station/<int:station_id>', methods=['GET'])
def get_station(station_id):
    """获取单个站点详情"""
    station = StationModel.get_by_id(station_id)
    if not station:
        return api_response(False, message="站点不存在")
    
    inventory = StationModel.get_inventory(station_id)
    
    return api_response(True, data={
        "station_id": station['station_id'],
        "name": station['name'],
        "pos_x": float(station['pos_x']) if station['pos_x'] else 0,
        "pos_y": float(station['pos_y']) if station['pos_y'] else 0,
        "status": station['status'],
        "inventory": inventory
    })

