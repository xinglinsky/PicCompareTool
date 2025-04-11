#include "mainwindow.h"
#include "configmanager.h"
#include "languagemanager.h"
#include <QApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("PicCompareTool");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("PicCompareTool");
    app.setOrganizationDomain("piccomparetool.example.com");
    
    // 设置翻译文件搜索路径
    QStringList translationPaths;
    translationPaths << QCoreApplication::applicationDirPath() + "/translations";
    translationPaths << QDir::currentPath() + "/translations";
    translationPaths << QDir::currentPath() + "/../translations";
    translationPaths << QDir::currentPath() + "/../src/translations";
    
    for (const QString &path : translationPaths) {
        QDir dir(path);
        if (dir.exists()) {
            qDebug() << "Found translation directory:" << path;
        }
    }
    
    // 初始化配置管理器并加载配置
    ConfigManager::instance()->loadConfig();
    
    // 应用语言设置
    Language language = ConfigManager::instance()->getLanguage();
    LanguageManager::instance()->applyLanguage(language);
    
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
