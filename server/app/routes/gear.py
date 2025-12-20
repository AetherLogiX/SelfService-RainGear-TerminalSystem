# -*- coding: utf-8 -*-
"""
雨具相关API路由
"""
from flask import Blueprint, request, current_app
from app.utils import api_response, require_json
from app.models import GearModel, BorrowModel

gear_bp = Blueprint('gear', __name__)


@gear_bp.route('/station/<int:station_id>/gears', methods=['GET'])
def get_station_gears(station_id):
    """获取站点的所有雨具（槽位状态）"""
    gears = GearModel.get_by_station(station_id)
    
    slots = []
    gear_map = {g['slot_id']: g for g in gears}
    
    for slot_id in range(1, 13):
        gear = gear_map.get(slot_id)
        expected_type = current_app.config['SLOT_TYPE_MAP'].get(slot_id, 1)
        type_name = current_app.config['GEAR_TYPE_NAMES'].get(expected_type, '未知')
        
        if gear:
            slots.append({
                "slot_id": slot_id,
                "gear_id": gear['gear_id'],
                "type_id": gear['type_id'],
                "type_name": type_name,
                "status": gear['status'],
                "has_gear": True
            })
        else:
            slots.append({
                "slot_id": slot_id,
                "gear_id": None,
                "type_id": expected_type,
                "type_name": type_name,
                "status": 0,
                "has_gear": False
            })
    
    return api_response(True, data=slots)


@gear_bp.route('/gear/<gear_id>', methods=['GET'])
def get_gear(gear_id):
    """获取雨具详情"""
    gear = GearModel.get_by_id(gear_id)
    if not gear:
        return api_response(False, message="雨具不存在")
    
    return api_response(True, data={
        "gear_id": gear['gear_id'],
        "type_id": gear['type_id'],
        "type_name": current_app.config['GEAR_TYPE_NAMES'].get(gear['type_id'], '未知'),
        "station_id": gear['station_id'],
        "slot_id": gear['slot_id'],
        "status": gear['status']
    })


@gear_bp.route('/borrow', methods=['POST'])
@require_json('user_id', 'gear_id', 'station_id', 'slot_id')
def borrow_gear():
    """借伞"""
    data = request.get_json()
    
    gear = GearModel.get_by_id(data['gear_id'])
    if not gear:
        return api_response(False, message="雨具不存在")
    
    deposit = current_app.config['DEPOSIT'].get(gear['type_id'], 20.0)
    
    success, message = BorrowModel.borrow_gear(
        data['user_id'],
        data['gear_id'],
        data['station_id'],
        data['slot_id'],
        deposit
    )
    
    if success:
        return api_response(True, data={"deposit": deposit}, message=message)
    else:
        return api_response(False, message=message)


@gear_bp.route('/return', methods=['POST'])
@require_json('user_id', 'gear_id', 'station_id', 'slot_id')
def return_gear():
    """还伞"""
    data = request.get_json()
    
    gear = GearModel.get_by_id(data['gear_id'])
    if not gear:
        return api_response(False, message="雨具不存在")
    
    # 检查槽位类型是否匹配
    expected_type = current_app.config['SLOT_TYPE_MAP'].get(data['slot_id'], 0)
    if gear['type_id'] != expected_type:
        borrowed_type_name = current_app.config['GEAR_TYPE_NAMES'].get(gear['type_id'], '未知')
        expected_type_name = current_app.config['GEAR_TYPE_NAMES'].get(expected_type, '未知')
        slot_range = {1: "1-4", 2: "5-8", 3: "9-10", 4: "11-12"}.get(gear['type_id'], "未知")
        return api_response(False, message=f"类型不匹配：您借的是【{borrowed_type_name}】，请归还到 {slot_range} 号槽位")
    
    success, cost, message = BorrowModel.return_gear(
        data['user_id'],
        data['gear_id'],
        data['station_id'],
        data['slot_id']
    )
    
    if success:
        return api_response(True, data={"cost": cost}, message=message)
    else:
        return api_response(False, message=message)


@gear_bp.route('/health', methods=['GET'])
def health_check():
    """健康检查"""
    return api_response(True, message="CampusRain Server is running")


@gear_bp.route('/config/deposit', methods=['GET'])
def get_deposit_config():
    """获取押金配置"""
    deposits = []
    for type_id, amount in current_app.config['DEPOSIT'].items():
        deposits.append({
            "type_id": type_id,
            "type_name": current_app.config['GEAR_TYPE_NAMES'].get(type_id, '未知'),
            "deposit": amount
        })
    return api_response(True, data=deposits)

