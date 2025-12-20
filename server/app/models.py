# -*- coding: utf-8 -*-
"""
CampusRain 数据模型和数据库操作
"""
from datetime import datetime
from typing import Optional, List, Dict, Tuple
from flask import current_app
from app.database import get_db_handler, get_db_connection


class UserModel:
    """用户模型"""
    
    @staticmethod
    def get_by_id_and_name(user_id: str, real_name: str) -> Optional[Dict]:
        """根据学号/工号和姓名查询用户"""
        with get_db_handler() as db:
            sql = """
                SELECT user_id, real_name, credit, role, is_active, password, avatar_id
                FROM users 
                WHERE user_id = %s AND real_name = %s 
                LIMIT 1
            """
            db.execute(sql, (user_id.strip(), real_name.strip()))
            return db.fetchone()
    
    @staticmethod
    def get_by_id(user_id: str) -> Optional[Dict]:
        """根据ID获取用户"""
        with get_db_handler() as db:
            sql = "SELECT * FROM users WHERE user_id = %s LIMIT 1"
            db.execute(sql, (user_id.strip(),))
            return db.fetchone()
    
    @staticmethod
    def get_all() -> List[Dict]:
        """获取所有用户"""
        with get_db_handler() as db:
            sql = "SELECT user_id, real_name, credit, role, is_active FROM users ORDER BY user_id"
            db.execute(sql)
            return db.fetchall()
    
    @staticmethod
    def verify_password(user_id: str, real_name: str, password: str) -> Optional[Dict]:
        """验证用户密码"""
        user = UserModel.get_by_id_and_name(user_id, real_name)
        if user and user['password'] == password:
            return user
        return None
    
    @staticmethod
    def set_password(user_id: str, new_password: str) -> bool:
        """设置密码并激活账户"""
        with get_db_handler() as db:
            sql = "UPDATE users SET password = %s, is_active = 1 WHERE user_id = %s"
            db.execute(sql, (new_password, user_id.strip()))
            return db.rowcount > 0
    
    @staticmethod
    def change_password(user_id: str, old_password: str, new_password: str) -> bool:
        """修改密码"""
        with get_db_handler() as db:
            sql = "SELECT password FROM users WHERE user_id = %s LIMIT 1"
            db.execute(sql, (user_id.strip(),))
            result = db.fetchone()
            if not result or result['password'] != old_password:
                return False
            sql = "UPDATE users SET password = %s WHERE user_id = %s"
            db.execute(sql, (new_password, user_id.strip()))
            return db.rowcount > 0
    
    @staticmethod
    def get_balance(user_id: str) -> Optional[float]:
        """获取用户余额"""
        with get_db_handler() as db:
            sql = "SELECT credit FROM users WHERE user_id = %s LIMIT 1"
            db.execute(sql, (user_id.strip(),))
            result = db.fetchone()
            return result['credit'] if result else None


class StationModel:
    """站点模型"""
    
    @staticmethod
    def get_all() -> List[Dict]:
        """获取所有站点"""
        with get_db_handler() as db:
            sql = "SELECT * FROM station ORDER BY station_id"
            db.execute(sql)
            return db.fetchall()
    
    @staticmethod
    def get_by_id(station_id: int) -> Optional[Dict]:
        """根据ID获取站点"""
        with get_db_handler() as db:
            sql = "SELECT * FROM station WHERE station_id = %s LIMIT 1"
            db.execute(sql, (station_id,))
            return db.fetchone()
    
    @staticmethod
    def get_inventory(station_id: int) -> Dict:
        """获取站点库存统计"""
        with get_db_handler() as db:
            sql = """
                SELECT 
                    COUNT(CASE WHEN type_id = 1 AND status = 1 THEN 1 END) as plastic_umbrella,
                    COUNT(CASE WHEN type_id = 2 AND status = 1 THEN 1 END) as windproof_umbrella,
                    COUNT(CASE WHEN type_id = 3 AND status = 1 THEN 1 END) as sunshade_umbrella,
                    COUNT(CASE WHEN type_id = 4 AND status = 1 THEN 1 END) as raincoat,
                    COUNT(CASE WHEN status = 1 THEN 1 END) as total_available,
                    COUNT(CASE WHEN status = 2 THEN 1 END) as total_borrowed,
                    COUNT(CASE WHEN status = 3 THEN 1 END) as total_damaged
                FROM raingear 
                WHERE station_id = %s
            """
            db.execute(sql, (station_id,))
            return db.fetchone()
    
    @staticmethod
    def get_all_inventory() -> Dict:
        """获取全局库存统计"""
        with get_db_handler() as db:
            sql = """
                SELECT 
                    COUNT(CASE WHEN status = 1 THEN 1 END) as total_available,
                    COUNT(CASE WHEN status = 2 THEN 1 END) as total_borrowed,
                    COUNT(CASE WHEN status = 3 THEN 1 END) as total_damaged,
                    COUNT(*) as total_gears
                FROM raingear
            """
            db.execute(sql)
            return db.fetchone()


class GearModel:
    """雨具模型"""
    
    @staticmethod
    def get_all() -> List[Dict]:
        """获取所有雨具"""
        with get_db_handler() as db:
            sql = """
                SELECT g.*, s.name as station_name
                FROM raingear g
                LEFT JOIN station s ON g.station_id = s.station_id
                ORDER BY g.gear_id
            """
            db.execute(sql)
            return db.fetchall()
    
    @staticmethod
    def get_by_station(station_id: int) -> List[Dict]:
        """获取站点的所有雨具"""
        with get_db_handler() as db:
            sql = """
                SELECT gear_id, type_id, station_id, slot_id, status
                FROM raingear 
                WHERE station_id = %s 
                ORDER BY slot_id
            """
            db.execute(sql, (station_id,))
            return db.fetchall()
    
    @staticmethod
    def get_by_id(gear_id: str) -> Optional[Dict]:
        """根据ID获取雨具"""
        with get_db_handler() as db:
            sql = "SELECT * FROM raingear WHERE gear_id = %s LIMIT 1"
            db.execute(sql, (gear_id,))
            return db.fetchone()
    
    @staticmethod
    def set_status(gear_id: str, status: int) -> bool:
        """设置雨具状态 (1:可用, 2:借出, 3:故障)"""
        with get_db_handler() as db:
            sql = "UPDATE raingear SET status = %s WHERE gear_id = %s"
            db.execute(sql, (status, gear_id))
            return db.rowcount > 0
    
    @staticmethod
    def get_damaged() -> List[Dict]:
        """获取所有故障雨具"""
        with get_db_handler() as db:
            sql = """
                SELECT g.*, s.name as station_name
                FROM raingear g
                LEFT JOIN station s ON g.station_id = s.station_id
                WHERE g.status = 3
                ORDER BY g.gear_id
            """
            db.execute(sql)
            return db.fetchall()


class BorrowModel:
    """借还记录模型"""
    
    @staticmethod
    def get_user_current_borrow(user_id: str) -> Optional[Dict]:
        """获取用户当前未归还的借出记录"""
        with get_db_handler() as db:
            sql = """
                SELECT record_id, user_id, gear_id, borrow_time, return_time, cost
                FROM record 
                WHERE user_id = %s AND return_time IS NULL 
                LIMIT 1
            """
            db.execute(sql, (user_id.strip(),))
            result = db.fetchone()
            if result and result.get('borrow_time'):
                result['borrow_time'] = result['borrow_time'].strftime('%Y-%m-%d %H:%M:%S')
            return result
    
    @staticmethod
    def get_all_current_borrows() -> List[Dict]:
        """获取所有当前借出记录"""
        with get_db_handler() as db:
            sql = """
                SELECT r.*, u.real_name, g.type_id
                FROM record r
                LEFT JOIN users u ON r.user_id = u.user_id
                LEFT JOIN raingear g ON r.gear_id = g.gear_id
                WHERE r.return_time IS NULL
                ORDER BY r.borrow_time DESC
            """
            db.execute(sql)
            records = db.fetchall()
            for r in records:
                if r.get('borrow_time'):
                    r['borrow_time'] = r['borrow_time'].strftime('%Y-%m-%d %H:%M:%S')
            return records
    
    @staticmethod
    def borrow_gear(user_id: str, gear_id: str, station_id: int, slot_id: int, deposit: float) -> Tuple[bool, str]:
        """借伞操作"""
        with get_db_connection() as conn:
            db = conn.cursor()
            try:
                # 检查用户余额
                db.execute("SELECT credit FROM users WHERE user_id = %s LIMIT 1", (user_id,))
                user = db.fetchone()
                if not user:
                    return False, "用户不存在"
                if user['credit'] < deposit:
                    return False, f"余额不足，需要押金 {deposit} 元"
                
                # 检查雨具是否可借
                db.execute(
                    "SELECT status FROM raingear WHERE gear_id = %s AND station_id = %s AND slot_id = %s LIMIT 1",
                    (gear_id, station_id, slot_id)
                )
                gear = db.fetchone()
                if not gear:
                    return False, "雨具不存在或位置不匹配"
                if gear['status'] != 1:
                    return False, "雨具不可借"
                
                # 检查用户是否已有借出
                db.execute(
                    "SELECT record_id FROM record WHERE user_id = %s AND return_time IS NULL LIMIT 1",
                    (user_id,)
                )
                if db.fetchone():
                    return False, "您还有未归还的雨具"
                
                # 扣除押金
                db.execute("UPDATE users SET credit = credit - %s WHERE user_id = %s", (deposit, user_id))
                
                # 更新雨具状态
                db.execute(
                    "UPDATE raingear SET status = 2, station_id = NULL, slot_id = NULL WHERE gear_id = %s",
                    (gear_id,)
                )
                
                # 创建记录
                db.execute(
                    "INSERT INTO record (user_id, gear_id, borrow_time, cost) VALUES (%s, %s, NOW(), 0.00)",
                    (user_id, gear_id)
                )
                
                conn.commit()
                return True, f"借伞成功，押金 {deposit} 元已扣除"
                
            except Exception as e:
                conn.rollback()
                return False, f"借伞失败: {str(e)}"
            finally:
                db.close()
    
    @staticmethod
    def return_gear(user_id: str, gear_id: str, station_id: int, slot_id: int) -> Tuple[bool, float, str]:
        """还伞操作"""
        with get_db_connection() as conn:
            db = conn.cursor()
            try:
                # 查找借出记录
                db.execute(
                    "SELECT record_id, borrow_time FROM record WHERE user_id = %s AND gear_id = %s AND return_time IS NULL LIMIT 1",
                    (user_id, gear_id)
                )
                record = db.fetchone()
                if not record:
                    return False, 0.0, "未找到借出记录"
                
                record_id = record['record_id']
                borrow_time = record['borrow_time']
                
                # 计算费用
                now = datetime.now()
                hours = (now - borrow_time).total_seconds() / 3600
                cost = 0.0
                if hours > current_app.config['FREE_HOURS']:
                    extra_hours = hours - current_app.config['FREE_HOURS']
                    cost = (extra_hours / 12) * 1.0
                    cost = max(0, cost)
                
                # 检查槽位
                db.execute(
                    "SELECT COUNT(*) as cnt FROM raingear WHERE station_id = %s AND slot_id = %s AND status = 1",
                    (station_id, slot_id)
                )
                if db.fetchone()['cnt'] > 0:
                    return False, 0.0, "该槽位已被占用"
                
                # 获取雨具类型计算押金
                db.execute("SELECT type_id FROM raingear WHERE gear_id = %s LIMIT 1", (gear_id,))
                gear = db.fetchone()
                if not gear:
                    return False, 0.0, "雨具信息不存在"
                deposit = current_app.config['DEPOSIT'].get(gear['type_id'], 20.0)
                
                # 更新雨具状态
                db.execute(
                    "UPDATE raingear SET status = 1, station_id = %s, slot_id = %s WHERE gear_id = %s",
                    (station_id, slot_id, gear_id)
                )
                
                # 退还押金
                refund = deposit - cost
                if refund > 0:
                    db.execute("UPDATE users SET credit = credit + %s WHERE user_id = %s", (refund, user_id))
                
                # 更新记录
                db.execute(
                    "UPDATE record SET return_time = NOW(), cost = %s WHERE record_id = %s",
                    (cost, record_id)
                )
                
                conn.commit()
                
                if cost > 0:
                    return True, cost, f"还伞成功，使用费用 {cost:.2f} 元"
                else:
                    return True, 0.0, "还伞成功，24小时内免费"
                
            except Exception as e:
                conn.rollback()
                return False, 0.0, f"还伞失败: {str(e)}"
            finally:
                db.close()
    
    @staticmethod
    def get_user_history(user_id: str, limit: int = 20) -> List[Dict]:
        """获取用户借还历史"""
        with get_db_handler() as db:
            sql = """
                SELECT r.record_id, r.gear_id, r.borrow_time, r.return_time, r.cost, g.type_id
                FROM record r
                LEFT JOIN raingear g ON r.gear_id = g.gear_id
                WHERE r.user_id = %s
                ORDER BY r.borrow_time DESC
                LIMIT %s
            """
            db.execute(sql, (user_id.strip(), limit))
            records = db.fetchall()
            for r in records:
                if r.get('borrow_time'):
                    r['borrow_time'] = r['borrow_time'].strftime('%Y-%m-%d %H:%M:%S')
                if r.get('return_time'):
                    r['return_time'] = r['return_time'].strftime('%Y-%m-%d %H:%M:%S')
                r['type_name'] = current_app.config['GEAR_TYPE_NAMES'].get(r.get('type_id'), '未知')
            return records
    
    @staticmethod
    def get_recent_records(limit: int = 50) -> List[Dict]:
        """获取最近的借还记录"""
        with get_db_handler() as db:
            sql = """
                SELECT r.*, u.real_name, g.type_id
                FROM record r
                LEFT JOIN users u ON r.user_id = u.user_id
                LEFT JOIN raingear g ON r.gear_id = g.gear_id
                ORDER BY r.borrow_time DESC
                LIMIT %s
            """
            db.execute(sql, (limit,))
            records = db.fetchall()
            for r in records:
                if r.get('borrow_time'):
                    r['borrow_time'] = r['borrow_time'].strftime('%Y-%m-%d %H:%M:%S')
                if r.get('return_time'):
                    r['return_time'] = r['return_time'].strftime('%Y-%m-%d %H:%M:%S')
                r['type_name'] = current_app.config['GEAR_TYPE_NAMES'].get(r.get('type_id'), '未知')
            return records
