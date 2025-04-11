#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QVariant>
#include "thememanager.h"
#include "languagemanager.h"

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    static ConfigManager* instance();
    
    // 保存配置
    void saveConfig();
    
    // 加载配置
    void loadConfig();
    
    // 获取/设置主题
    Theme getTheme() const;
    void setTheme(Theme theme);
    
    // 获取/设置窗口大小
    QSize getWindowSize() const;
    void setWindowSize(const QSize &size);
    
    // 获取/设置窗口位置
    QPoint getWindowPosition() const;
    void setWindowPosition(const QPoint &pos);
    
    // 获取/设置比较模式
    int getComparisonMode() const;
    void setComparisonMode(int mode);
    
    // 获取/设置是否最大化
    bool isMaximized() const;
    void setMaximized(bool maximized);
    
    // 获取/设置上次打开的文件目录
    QString getLastOpenDirectory() const;
    void setLastOpenDirectory(const QString &directory);
    
    // 获取/设置语言
    Language getLanguage() const;
    void setLanguage(Language language);
    
signals:
    void configChanged();
    
private:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();
    
    // 初始化配置目录
    void initConfigDirectory();
    
    // 配置文件路径
    QString m_configFilePath;
    
    // 配置对象
    QSettings *m_settings;
    
    // 当前配置
    Theme m_theme;
    QSize m_windowSize;
    QPoint m_windowPosition;
    int m_comparisonMode;
    bool m_isMaximized;
    QString m_lastOpenDirectory;
    Language m_language;
    
    // 单例实例
    static ConfigManager* m_instance;
};

#endif // CONFIGMANAGER_H
