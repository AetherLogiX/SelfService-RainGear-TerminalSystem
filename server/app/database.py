# -*- coding: utf-8 -*-
"""
CampusRain 数据库连接管理
"""
import pymysql
from contextlib import contextmanager
from flask import current_app


def get_connection():
    """获取数据库连接"""
    return pymysql.connect(
        host=current_app.config['MYSQL_HOST'],
        port=current_app.config['MYSQL_PORT'],
        user=current_app.config['MYSQL_USER'],
        password=current_app.config['MYSQL_PASSWORD'],
        database=current_app.config['MYSQL_DATABASE'],
        charset='utf8mb4',
        cursorclass=pymysql.cursors.DictCursor
    )


@contextmanager
def get_db_handler():
    """获取数据库操作句柄的上下文管理器"""
    conn = get_connection()
    try:
        handler = conn.cursor()
        yield handler
        conn.commit()
    except Exception as e:
        conn.rollback()
        raise e
    finally:
        handler.close()
        conn.close()


@contextmanager
def get_db_connection():
    """获取数据库连接的上下文管理器（用于事务）"""
    conn = get_connection()
    try:
        yield conn
        conn.commit()
    except Exception as e:
        conn.rollback()
        raise e
    finally:
        conn.close()
