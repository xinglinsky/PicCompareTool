#include "thememanager.h"
#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QDebug>
#include <QResource>

// 初始化静态成员
ThemeManager* ThemeManager::m_instance = nullptr;

ThemeManager* ThemeManager::instance()
{
    if (!m_instance) {
        m_instance = new ThemeManager(qApp);
    }
    return m_instance;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme(Theme::LightTheme)
{
    // 设置主题文件映射
    m_themeFiles[Theme::LightTheme] = ":/themes/light.qss";
    m_themeFiles[Theme::DarkTheme] = ":/themes/dark.qss";
}

ThemeManager::~ThemeManager()
{
    m_instance = nullptr;
}

void ThemeManager::applyTheme(Theme theme)
{
    if (m_currentTheme == theme) {
        return;  // 如果主题没有变化，直接返回
    }
    
    m_currentTheme = theme;
    
    QString styleSheet = loadStyleSheet(m_themeFiles[theme]);
    qApp->setStyleSheet(styleSheet);
    
    emit themeChanged(theme);
}

Theme ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

QString ThemeManager::getThemeName(Theme theme) const
{
    switch (theme) {
        case Theme::LightTheme:
            return tr("Light");
        case Theme::DarkTheme:
            return tr("Dark");
        default:
            return tr("Unknown");
    }
}

QString ThemeManager::loadStyleSheet(const QString &fileName) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to load stylesheet:" << fileName;
        return QString();
    }
    
    return QString::fromUtf8(file.readAll());
}
