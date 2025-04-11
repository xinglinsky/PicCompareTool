#include "configmanager.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QApplication>

// 初始化静态成员
ConfigManager* ConfigManager::m_instance = nullptr;

ConfigManager* ConfigManager::instance()
{
    if (!m_instance) {
        m_instance = new ConfigManager(qApp);
    }
    return m_instance;
}

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_theme(Theme::LightTheme)
    , m_windowSize(1200, 800)
    , m_windowPosition(100, 100)
    , m_comparisonMode(0)
    , m_isMaximized(false)
    , m_lastOpenDirectory(QDir::homePath())
    , m_language(Language::English)
{
    // 初始化配置目录和文件
    initConfigDirectory();
    
    // 创建QSettings对象
    m_settings = new QSettings(m_configFilePath, QSettings::IniFormat, this);
}

ConfigManager::~ConfigManager()
{
    saveConfig();
    m_instance = nullptr;
}

void ConfigManager::initConfigDirectory()
{
    // 获取AppData路径
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    
    // 创建应用程序目录
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // 设置配置文件路径
    m_configFilePath = appDataPath + "/config.ini";
    
    qDebug() << "Config file path:" << m_configFilePath;
}

void ConfigManager::saveConfig()
{
    // 保存主题
    m_settings->setValue("Theme", static_cast<int>(m_theme));
    
    // 保存窗口大小和位置
    m_settings->setValue("WindowWidth", m_windowSize.width());
    m_settings->setValue("WindowHeight", m_windowSize.height());
    m_settings->setValue("WindowX", m_windowPosition.x());
    m_settings->setValue("WindowY", m_windowPosition.y());
    
    // 保存比较模式
    m_settings->setValue("ComparisonMode", m_comparisonMode);
    
    // 保存是否最大化
    m_settings->setValue("IsMaximized", m_isMaximized);
    
    // 保存上次打开的文件目录
    m_settings->setValue("LastOpenDirectory", m_lastOpenDirectory);
    
    // 保存语言设置
    m_settings->setValue("Language", static_cast<int>(m_language));
    
    // 同步到文件
    m_settings->sync();
    
    qDebug() << "Configuration saved to" << m_configFilePath;
}

void ConfigManager::loadConfig()
{
    // 检查配置文件是否存在
    QFile configFile(m_configFilePath);
    if (!configFile.exists()) {
        qDebug() << "Config file does not exist, using default settings";
        return;
    }
    
    // 加载主题
    int themeValue = m_settings->value("Theme", static_cast<int>(Theme::LightTheme)).toInt();
    m_theme = static_cast<Theme>(themeValue);
    
    // 加载窗口大小和位置
    int width = m_settings->value("WindowWidth", 1200).toInt();
    int height = m_settings->value("WindowHeight", 800).toInt();
    m_windowSize = QSize(width, height);
    
    int x = m_settings->value("WindowX", 100).toInt();
    int y = m_settings->value("WindowY", 100).toInt();
    m_windowPosition = QPoint(x, y);
    
    // 加载比较模式
    m_comparisonMode = m_settings->value("ComparisonMode", 0).toInt();
    
    // 加载是否最大化
    m_isMaximized = m_settings->value("IsMaximized", false).toBool();
    
    // 加载上次打开的文件目录
    m_lastOpenDirectory = m_settings->value("LastOpenDirectory", QDir::homePath()).toString();
    
    // 加载语言设置
    int languageValue = m_settings->value("Language", static_cast<int>(Language::English)).toInt();
    m_language = static_cast<Language>(languageValue);
    
    qDebug() << "Configuration loaded from" << m_configFilePath;
    
    // 发送配置已更改信号
    emit configChanged();
}

Theme ConfigManager::getTheme() const
{
    return m_theme;
}

void ConfigManager::setTheme(Theme theme)
{
    if (m_theme != theme) {
        m_theme = theme;
        emit configChanged();
    }
}

QSize ConfigManager::getWindowSize() const
{
    return m_windowSize;
}

void ConfigManager::setWindowSize(const QSize &size)
{
    if (m_windowSize != size) {
        m_windowSize = size;
        emit configChanged();
    }
}

QPoint ConfigManager::getWindowPosition() const
{
    return m_windowPosition;
}

void ConfigManager::setWindowPosition(const QPoint &pos)
{
    if (m_windowPosition != pos) {
        m_windowPosition = pos;
        emit configChanged();
    }
}

int ConfigManager::getComparisonMode() const
{
    return m_comparisonMode;
}

void ConfigManager::setComparisonMode(int mode)
{
    if (m_comparisonMode != mode) {
        m_comparisonMode = mode;
        emit configChanged();
    }
}

bool ConfigManager::isMaximized() const
{
    return m_isMaximized;
}

void ConfigManager::setMaximized(bool maximized)
{
    if (m_isMaximized != maximized) {
        m_isMaximized = maximized;
        emit configChanged();
    }
}

QString ConfigManager::getLastOpenDirectory() const
{
    return m_lastOpenDirectory;
}

void ConfigManager::setLastOpenDirectory(const QString &directory)
{
    if (m_lastOpenDirectory != directory) {
        m_lastOpenDirectory = directory;
        emit configChanged();
    }
}

Language ConfigManager::getLanguage() const
{
    return m_language;
}

void ConfigManager::setLanguage(Language language)
{
    if (m_language != language) {
        m_language = language;
        emit configChanged();
    }
}
