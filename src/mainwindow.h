#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QComboBox>
#include <QSplitter>
#include <QClipboard>

#include "imageviewer.h"
#include "imageloader.h"
#include "titlebar.h"
#include "thememanager.h"
#include "configmanager.h"
#include "iconmanager.h"
#include "languagemanager.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openImageLeft();
    void openImageRight();
    void pasteFromClipboardLeft();
    void pasteFromClipboardRight();
    void loadFromUrlLeft();
    void loadFromUrlRight();
    void switchComparisonMode(int index);
    void updateStatusBar();
    void saveSettings();
    void loadSettings();
    void applyTheme(Theme theme);
    void onThemeChanged(Theme theme);
    void updateActionIcons();
    void onLanguageChanged(Language language);
    void applyLanguage(Language language);

private:
    void createActions();
    void createToolBars();
    void createStatusBar();
    void setupLayout();
    void setupConnections();
    void setupTitleBar();

    // UI components
    TitleBar *m_titleBar;
    QSplitter *m_splitter;
    ImageViewer *m_leftViewer;
    ImageViewer *m_rightViewer;
    QComboBox *m_modeComboBox;
    QLabel *m_leftImageInfoLabel;
    QLabel *m_rightImageInfoLabel;
    QToolBar *m_mainToolBar;
    
    // Actions
    QAction *m_openLeftAction;
    QAction *m_openRightAction;
    QAction *m_pasteLeftAction;
    QAction *m_pasteRightAction;
    QAction *m_urlLeftAction;
    QAction *m_urlRightAction;
    QAction *m_lightThemeAction;
    QAction *m_darkThemeAction;
    
    // 语言相关的Actions
    QAction *m_englishAction;
    QAction *m_chineseAction;
    QAction *m_japaneseAction;
    QAction *m_koreanAction;
    
    // Utilities
    ImageLoader *m_imageLoader;
    
    // Current state
    ComparisonMode m_currentMode;
    Theme m_currentTheme;
    Language m_currentLanguage;
};

#endif // MAINWINDOW_H
