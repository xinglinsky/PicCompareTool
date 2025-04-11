#include "iconmanager.h"
#include <QApplication>
#include <QDebug>
#include <QDir>

// 初始化静态成员
IconManager* IconManager::m_instance = nullptr;

IconManager* IconManager::instance()
{
    if (!m_instance) {
        m_instance = new IconManager(qApp);
    }
    return m_instance;
}

IconManager::IconManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme(ThemeManager::instance()->currentTheme())
{
    // 加载初始图标
    loadIcons();
    
    // 连接主题变更信号
    connect(ThemeManager::instance(), &ThemeManager::themeChanged, 
            this, &IconManager::onThemeChanged);
}

IconManager::~IconManager()
{
    m_instance = nullptr;
}

QIcon IconManager::getIcon(IconType iconType) const
{
    if (m_icons.contains(iconType)) {
        return m_icons[iconType];
    }
    
    // 如果找不到图标，返回一个空图标
    return QIcon();
}

QString IconManager::getIconPath(IconType iconType) const
{
    // 直接调用私有方法获取当前主题下的图标路径
    return getIconPath(iconType, m_currentTheme);
}

void IconManager::updateIcons()
{
    // 更新当前主题
    m_currentTheme = ThemeManager::instance()->currentTheme();
    
    // 重新加载图标
    loadIcons();
}

void IconManager::onThemeChanged(Theme theme)
{
    m_currentTheme = theme;
    loadIcons();
}

void IconManager::loadIcons()
{
    // 清空当前图标缓存
    m_icons.clear();
    
    // 加载各种图标
    m_icons[IconType::Open] = QIcon(getIconPath(IconType::Open, m_currentTheme));
    m_icons[IconType::Paste] = QIcon(getIconPath(IconType::Paste, m_currentTheme));
    m_icons[IconType::Url] = QIcon(getIconPath(IconType::Url, m_currentTheme));
    m_icons[IconType::ComboArrow] = QIcon(getIconPath(IconType::ComboArrow, m_currentTheme));
    
    qDebug() << "Icons loaded for theme:" << (m_currentTheme == Theme::LightTheme ? "Light" : "Dark");
}

QString IconManager::getIconPath(IconType iconType, Theme theme) const
{
    // 确定主题文件夹
    QString themeFolder = (theme == Theme::LightTheme) ? "light" : "dark";
    QString basePath = ":/icons/" + themeFolder + "/";
    
    // 根据图标类型返回对应的文件名
    switch (iconType) {
        case IconType::Open:
            return basePath + "open.png";
        case IconType::Paste:
            return basePath + "paste.png";
        case IconType::Url:
            return basePath + "url.png";
        case IconType::ComboArrow:
            return basePath + "combo-arrow.png";
        default:
            return QString();
    }
}
