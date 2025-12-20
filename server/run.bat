@echo off
chcp 65001 >nul
echo ============================================
echo   CampusRain - Flask Server Startup
echo ============================================

cd /d "%~dp0"

REM 设置Python路径
set PYTHON_PATH=E:\python3.14\python.exe

REM 检查Python是否存在
if not exist "%PYTHON_PATH%" (
    echo [ERROR] Python not found at %PYTHON_PATH%
    echo Please update PYTHON_PATH in this script.
    pause
    exit /b 1
)

echo [INFO] Using Python: %PYTHON_PATH%
echo [INFO] Starting Flask server...
echo.
echo [INFO] API Address: http://127.0.0.1:5000/api/
echo [INFO] Admin Panel: http://127.0.0.1:5000/admin/
echo.

REM 延迟2秒后打开浏览器（让服务器有时间启动）
start "" cmd /c "timeout /t 2 /nobreak >nul && start http://127.0.0.1:5000/admin/"

REM 启动Flask服务器
"%PYTHON_PATH%" run.py

pause
