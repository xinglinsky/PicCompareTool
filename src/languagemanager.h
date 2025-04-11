#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QObject>
#include <QTranslator>
#include <QMap>
#include <QString>
#include <QLocale>
#include <QApplication>

// 定义语言枚举
enum class Language {
    English,    // 英语
    Chinese,    // 中文
    Japanese,   // 日语
    Korean      // 韩语
};

class LanguageManager : public QObject
{
    Q_OBJECT

public:
    static LanguageManager* instance();
    
    // 应用语言
    void applyLanguage(Language language);
    
    // 获取当前语言
    Language currentLanguage() const;
    
    // 获取语言名称
    QString getLanguageName(Language language) const;
    
    // 获取语言代码
    QString getLanguageCode(Language language) const;
    
    // 获取所有支持的语言
    QList<Language> supportedLanguages() const;
    
signals:
    void languageChanged(Language language);
    
private:
    explicit LanguageManager(QObject *parent = nullptr);
    ~LanguageManager();
    
    // 加载翻译文件
    bool loadTranslation(const QString &locale);
    
    // 当前语言
    Language m_currentLanguage;
    
    // 翻译器
    QTranslator m_translator;
    
    // 语言代码映射
    QMap<Language, QString> m_languageCodes;
    
    // 单例实例
    static LanguageManager* m_instance;
};

#endif // LANGUAGEMANAGER_H
