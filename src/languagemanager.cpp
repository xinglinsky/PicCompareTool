#include "languagemanager.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

// 初始化静态成员
LanguageManager* LanguageManager::m_instance = nullptr;

LanguageManager* LanguageManager::instance()
{
    if (!m_instance) {
        m_instance = new LanguageManager(qApp);
    }
    return m_instance;
}

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent)
    , m_currentLanguage(Language::English)
{
    // 初始化语言代码映射
    m_languageCodes[Language::English] = "en";
    m_languageCodes[Language::Chinese] = "zh_CN";
    m_languageCodes[Language::Japanese] = "ja";
    m_languageCodes[Language::Korean] = "ko";
}

LanguageManager::~LanguageManager()
{
    m_instance = nullptr;
}

void LanguageManager::applyLanguage(Language language)
{
    if (m_currentLanguage == language) {
        return;  // 如果语言没有变化，直接返回
    }
    
    // 获取语言代码
    QString languageCode = m_languageCodes[language];
    
    // 加载翻译
    if (loadTranslation(languageCode)) {
        m_currentLanguage = language;
        emit languageChanged(language);
        qDebug() << "Language changed to:" << getLanguageName(language);
    } else {
        qWarning() << "Failed to load translation for language:" << getLanguageName(language);
    }
}

Language LanguageManager::currentLanguage() const
{
    return m_currentLanguage;
}

QString LanguageManager::getLanguageName(Language language) const
{
    switch (language) {
        case Language::English:
            return tr("English");
        case Language::Chinese:
            return tr("Chinese");
        case Language::Japanese:
            return tr("Japanese");
        case Language::Korean:
            return tr("Korean");
        default:
            return tr("Unknown");
    }
}

QString LanguageManager::getLanguageCode(Language language) const
{
    return m_languageCodes.value(language, "en");
}

QList<Language> LanguageManager::supportedLanguages() const
{
    return m_languageCodes.keys();
}

bool LanguageManager::loadTranslation(const QString &locale)
{
    // 移除旧的翻译
    qApp->removeTranslator(&m_translator);
    
    // 如果是英语，不需要加载翻译文件
    if (locale == "en") {
        return true;
    }
    
    // 尝试从多个位置加载翻译文件
    QStringList searchPaths;
    
    // 1. 应用程序目录下的translations文件夹
    searchPaths << QCoreApplication::applicationDirPath() + "/translations/";
    
    // 2. 当前工作目录下的translations文件夹
    searchPaths << QDir::currentPath() + "/translations/";
    
    // 3. 构建目录下的translations文件夹
    searchPaths << QDir::currentPath() + "/../translations/";
    
    // 4. 源代码目录下的translations文件夹
    searchPaths << QDir::currentPath() + "/../src/translations/";
    
    // 翻译文件名
    QString fileName = "piccomparetool_" + locale + ".qm";
    
    // 在各个路径中查找翻译文件
    for (const QString &path : searchPaths) {
        QString filePath = path + fileName;
        QFileInfo fileInfo(filePath);
        
        qDebug() << "Trying to load translation from:" << filePath;
        
        if (fileInfo.exists() && m_translator.load(filePath)) {
            qApp->installTranslator(&m_translator);
            qDebug() << "Successfully loaded translation from:" << filePath;
            return true;
        }
    }
    
    // 尝试从资源中加载
    QString resourcePath = ":/translations/" + fileName;
    if (m_translator.load(resourcePath)) {
        qApp->installTranslator(&m_translator);
        qDebug() << "Successfully loaded translation from resource:" << resourcePath;
        return true;
    }
    
    qWarning() << "Failed to load translation for locale:" << locale;
    return false;
}
