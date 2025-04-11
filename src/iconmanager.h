#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include <QObject>
#include <QIcon>
#include <QMap>
#include "thememanager.h"

// 定义图标类型枚举
enum class IconType {
    Open,
    Paste,
    Url,
    ComboArrow
};

class IconManager : public QObject
{
    Q_OBJECT

public:
    static IconManager* instance();
    
    // 获取当前主题下的图标
    QIcon getIcon(IconType iconType) const;
    
    // 获取当前主题下的图标路径
    QString getIconPath(IconType iconType) const;
    
    // 更新所有图标
    void updateIcons();
    
private slots:
    // 主题变更时更新图标
    void onThemeChanged(Theme theme);
    
private:
    explicit IconManager(QObject *parent = nullptr);
    ~IconManager();
    
    // 加载图标
    void loadIcons();
    
    // 获取图标路径
    QString getIconPath(IconType iconType, Theme theme) const;
    
    // 当前主题
    Theme m_currentTheme;
    
    // 图标缓存
    QMap<IconType, QIcon> m_icons;
    
    // 单例实例
    static IconManager* m_instance;
};

#endif // ICONMANAGER_H
