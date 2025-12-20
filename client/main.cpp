#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QSqlDatabase>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 设置Qt插件路径，让Qt能找到sqldrivers
    QString appDir = QCoreApplication::applicationDirPath();
    // Qt期望插件在plugins子目录下
    QString pluginPath1 = QDir(appDir).absoluteFilePath("plugins");
    QString pluginPath2 = QDir(appDir).absoluteFilePath("sqldrivers");
    QCoreApplication::addLibraryPath(pluginPath1);
    QCoreApplication::addLibraryPath(pluginPath2);
    qDebug() << "[Main] Application dir:" << appDir;
    qDebug() << "[Main] Plugin path 1 (plugins):" << pluginPath1 << QDir(pluginPath1).exists();
    qDebug() << "[Main] Plugin path 2 (sqldrivers):" << pluginPath2 << QDir(pluginPath2).exists();
    qDebug() << "[Main] All library paths:" << QCoreApplication::libraryPaths();
    qDebug() << "[Main] Available SQL drivers:" << QSqlDatabase::drivers();
    
    MainWindow w;
    w.show();
    return app.exec();
}

