# -*- coding: utf-8 -*-
"""
用户相关API路由
"""
from flask import Blueprint, request
from app.utils import api_response, require_json
from app.models import UserModel, BorrowModel, GearModel
from flask import current_app

user_bp = Blueprint('user', __name__)


@user_bp.route('/verify', methods=['POST'])
@require_json('user_id', 'real_name')
def verify_user():
    """验证用户是否存在"""
    data = request.get_json()
    user = UserModel.get_by_id_and_name(data['user_id'], data['real_name'])
    
    if not user:
        return api_response(False, message="用户不存在，请检查学号/工号和姓名")
    
    return api_response(True, data={
        "user_id": user['user_id'],
        "real_name": user['real_name'],
        "role": user['role'],
        "is_active": bool(user['is_active']),
        "credit": float(user['credit'])
    }, message="用户验证成功")


@user_bp.route('/login', methods=['POST'])
@require_json('user_id', 'real_name', 'password')
def login():
    """用户登录"""
    data = request.get_json()
    user = UserModel.verify_password(data['user_id'], data['real_name'], data['password'])
    
    if not user:
        return api_response(False, message="密码错误")
    
    return api_response(True, data={
        "user_id": user['user_id'],
        "real_name": user['real_name'],
        "role": user['role'],
        "credit": float(user['credit'])
    }, message="登录成功")


@user_bp.route('/activate', methods=['POST'])
@require_json('user_id', 'new_password')
def activate_user():
    """激活用户"""
    data = request.get_json()
    
    if len(data['new_password']) < 6:
        return api_response(False, message="密码长度至少6位")
    
    success = UserModel.set_password(data['user_id'], data['new_password'])
    
    if success:
        return api_response(True, message="账户激活成功")
    else:
        return api_response(False, message="激活失败，用户不存在")


@user_bp.route('/change_password', methods=['POST'])
@require_json('user_id', 'old_password', 'new_password')
def change_password():
    """修改密码"""
    data = request.get_json()
    
    if len(data['new_password']) < 6:
        return api_response(False, message="新密码长度至少6位")
    
    success = UserModel.change_password(data['user_id'], data['old_password'], data['new_password'])
    
    if success:
        return api_response(True, message="密码修改成功")
    else:
        return api_response(False, message="旧密码错误")


@user_bp.route('/balance', methods=['GET'])
def get_balance():
    """获取用户余额"""
    user_id = request.args.get('user_id')
    if not user_id:
        return api_response(False, message="缺少user_id参数", code=400)
    
    balance = UserModel.get_balance(user_id)
    if balance is None:
        return api_response(False, message="用户不存在")
    
    return api_response(True, data={"credit": float(balance)})


@user_bp.route('/current_borrow', methods=['GET'])
def get_current_borrow():
    """获取用户当前借出的雨具"""
    user_id = request.args.get('user_id')
    if not user_id:
        return api_response(False, message="缺少user_id参数", code=400)
    
    borrow = BorrowModel.get_user_current_borrow(user_id)
    
    if not borrow:
        return api_response(True, data=None, message="当前没有借出的雨具")
    
    gear = GearModel.get_by_id(borrow['gear_id'])
    if gear:
        borrow['type_id'] = gear['type_id']
        borrow['type_name'] = current_app.config['GEAR_TYPE_NAMES'].get(gear['type_id'], '未知')
    
    return api_response(True, data=borrow)


@user_bp.route('/history', methods=['GET'])
def get_borrow_history():
    """获取用户借还历史"""
    user_id = request.args.get('user_id')
    limit = request.args.get('limit', 20, type=int)
    
    if not user_id:
        return api_response(False, message="缺少user_id参数", code=400)
    
    records = BorrowModel.get_user_history(user_id, limit)
    return api_response(True, data=records)

