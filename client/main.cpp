#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QSqlDatabase>
#include <QFont>
#include <QMessageBox>
#include "MainWindow.h"
#include "Styles.h"
#include "ApiClient.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 设置Qt插件路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString pluginPath1 = QDir(appDir).absoluteFilePath("plugins");
    QString pluginPath2 = QDir(appDir).absoluteFilePath("sqldrivers");
    QCoreApplication::addLibraryPath(pluginPath1);
    QCoreApplication::addLibraryPath(pluginPath2);
    qDebug() << "[Main] Application dir:" << appDir;
    qDebug() << "[Main] Plugin path 1 (plugins):" << pluginPath1 << QDir(pluginPath1).exists();
    qDebug() << "[Main] Plugin path 2 (sqldrivers):" << pluginPath2 << QDir(pluginPath2).exists();
    qDebug() << "[Main] All library paths:" << QCoreApplication::libraryPaths();
    qDebug() << "[Main] Available SQL drivers:" << QSqlDatabase::drivers();
    
    // 初始化API客户端（分布式模式）
    // 可以通过命令行参数或配置文件修改服务器地址
    QString serverUrl = "http://127.0.0.1:5000";
    
    // 检查命令行参数
    QStringList args = QCoreApplication::arguments();
    for (int i = 1; i < args.size(); ++i) {
        if (args[i].startsWith("--server=")) {
            serverUrl = args[i].mid(9);
        }
    }
    
    ApiClient::instance().setServerUrl(serverUrl);
    qDebug() << "[Main] API Server:" << serverUrl;
    
    // 检查服务器连接
    if (!ApiClient::instance().checkConnection()) {
        qWarning() << "[Main] 无法连接到服务器，请确保服务器正在运行";
        QMessageBox::warning(nullptr, "连接失败", 
            QString("无法连接到服务器 %1\n\n请确保：\n1. Flask服务器正在运行\n2. 服务器地址正确\n\n启动服务器：\ncd server && python run.py").arg(serverUrl));
        // 继续运行，让用户可以在UI中看到错误
    } else {
        qDebug() << "[Main] 服务器连接成功";
    }
    
    // 设置全局字体
    QFont globalFont("Microsoft YaHei UI", 10);
    globalFont.setStyleStrategy(QFont::PreferAntialias);
    app.setFont(globalFont);
    
    // 应用全局样式
    app.setStyleSheet(Styles::globalStyle());
    
    MainWindow w;
    w.setWindowTitle("CampusRain - 校园智能共享雨具系统");
    w.resize(1000, 750);
    w.show();
    
    return app.exec();
}
