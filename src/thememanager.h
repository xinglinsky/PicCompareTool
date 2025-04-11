#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QApplication>
#include <QFile>
#include <QDir>

enum class Theme { LightTheme, DarkTheme };

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    static ThemeManager* instance();
    
    void applyTheme(Theme theme);
    Theme currentTheme() const;
    
    // 获取主题名称
    QString getThemeName(Theme theme) const;
    
signals:
    void themeChanged(Theme theme);
    
private:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();
    
    // 加载QSS文件
    QString loadStyleSheet(const QString &fileName) const;
    
    // 当前主题
    Theme m_currentTheme;
    
    // 主题文件路径映射
    QMap<Theme, QString> m_themeFiles;
    
    // 单例实例
    static ThemeManager* m_instance;
};

#endif // THEMEMANAGER_H
