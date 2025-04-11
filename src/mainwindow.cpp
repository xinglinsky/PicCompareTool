#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QMenuBar>
#include <QToolButton>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_leftViewer(new ImageViewer(this))
    , m_rightViewer(new ImageViewer(this))
    , m_imageLoader(new ImageLoader(this))
    , m_currentMode(ComparisonMode::SideBySide)
    , m_currentTheme(Theme::LightTheme)
    , m_currentLanguage(Language::English)
{
    // Remove window frame and menu bar
    setWindowFlags(Qt::FramelessWindowHint);
    menuBar()->setVisible(false);
    
    setupTitleBar();
    createActions();
    createToolBars();
    createStatusBar();
    setupLayout();
    setupConnections();
    
    // 加载保存的设置
    loadSettings();
    
    // 应用初始主题
    ThemeManager::instance()->applyTheme(m_currentTheme);
    
    // 应用初始语言
    LanguageManager::instance()->applyLanguage(m_currentLanguage);
    
    resize(1200, 800);
    setWindowTitle(tr("Image Comparison Tool"));
}

void MainWindow::createActions()
{
    // 获取图标管理器实例
    IconManager *iconManager = IconManager::instance();
    
    // File menu actions
    m_openLeftAction = new QAction(iconManager->getIcon(IconType::Open), tr("Open Left Image..."), this);
    m_openLeftAction->setShortcut(QKeySequence::Open);
    m_openLeftAction->setStatusTip(tr("Open an image file in the left panel"));
    
    m_openRightAction = new QAction(iconManager->getIcon(IconType::Open), tr("Open Right Image..."), this);
    m_openRightAction->setStatusTip(tr("Open an image file in the right panel"));
    
    // Edit menu actions
    m_pasteLeftAction = new QAction(iconManager->getIcon(IconType::Paste), tr("Paste to Left"), this);
    m_pasteLeftAction->setShortcut(QKeySequence("Ctrl+Shift+V"));
    m_pasteLeftAction->setStatusTip(tr("Paste image from clipboard to left panel"));
    
    m_pasteRightAction = new QAction(iconManager->getIcon(IconType::Paste), tr("Paste to Right"), this);
    m_pasteRightAction->setShortcut(QKeySequence("Ctrl+Alt+V"));
    m_pasteRightAction->setStatusTip(tr("Paste image from clipboard to right panel"));
    
    // URL actions
    m_urlLeftAction = new QAction(iconManager->getIcon(IconType::Url), tr("Load URL to Left..."), this);
    m_urlLeftAction->setStatusTip(tr("Load image from URL to left panel"));
    
    m_urlRightAction = new QAction(iconManager->getIcon(IconType::Url), tr("Load URL to Right..."), this);
    m_urlRightAction->setStatusTip(tr("Load image from URL to right panel"));
    
    // Theme actions
    m_lightThemeAction = new QAction(tr("Light"), this);
    m_lightThemeAction->setCheckable(true);
    connect(m_lightThemeAction, &QAction::triggered, this, [this]() {
        ThemeManager::instance()->applyTheme(Theme::LightTheme);
    });
    
    m_darkThemeAction = new QAction(tr("Dark"), this);
    m_darkThemeAction->setCheckable(true);
    connect(m_darkThemeAction, &QAction::triggered, this, [this]() {
        ThemeManager::instance()->applyTheme(Theme::DarkTheme);
    });
    
    // Language actions
    m_englishAction = new QAction(tr("English"), this);
    m_englishAction->setCheckable(true);
    connect(m_englishAction, &QAction::triggered, this, [this]() {
        LanguageManager::instance()->applyLanguage(Language::English);
    });
    
    m_chineseAction = new QAction(tr("Chinese"), this);
    m_chineseAction->setCheckable(true);
    connect(m_chineseAction, &QAction::triggered, this, [this]() {
        LanguageManager::instance()->applyLanguage(Language::Chinese);
    });
    
    m_japaneseAction = new QAction(tr("Japanese"), this);
    m_japaneseAction->setCheckable(true);
    connect(m_japaneseAction, &QAction::triggered, this, [this]() {
        LanguageManager::instance()->applyLanguage(Language::Japanese);
    });
    
    m_koreanAction = new QAction(tr("Korean"), this);
    m_koreanAction->setCheckable(true);
    connect(m_koreanAction, &QAction::triggered, this, [this]() {
        LanguageManager::instance()->applyLanguage(Language::Korean);
    });
}

void MainWindow::setupTitleBar()
{
    m_titleBar = new TitleBar(this);
    m_titleBar->setTitle(tr("Image Comparison Tool"));
    
    // Connect title bar signals
    connect(m_titleBar, &TitleBar::minimizeClicked, this, [this]() {
        setWindowState(windowState() | Qt::WindowMinimized);
    });
    
    connect(m_titleBar, &TitleBar::maximizeClicked, this, [this]() {
        if (windowState() & Qt::WindowMaximized) {
            setWindowState(windowState() & ~Qt::WindowMaximized);
        } else {
            setWindowState(windowState() | Qt::WindowMaximized);
        }
    });
    
    connect(m_titleBar, &TitleBar::closeClicked, this, &MainWindow::close);
    
    connect(m_titleBar, &TitleBar::settingsClicked, this, [this]() {
        // 创建设置菜单
        QMenu *settingsMenu = new QMenu(this);
        
        // 添加主题子菜单
        QMenu *themeMenu = settingsMenu->addMenu(tr("Theme"));
        themeMenu->addAction(m_lightThemeAction);
        themeMenu->addAction(m_darkThemeAction);
        
        // 添加语言子菜单
        QMenu *languageMenu = settingsMenu->addMenu(tr("Language"));
        languageMenu->addAction(m_englishAction);
        languageMenu->addAction(m_chineseAction);
        languageMenu->addAction(m_japaneseAction);
        languageMenu->addAction(m_koreanAction);
        
        // 显示菜单
        settingsMenu->exec(QCursor::pos());
        settingsMenu->deleteLater();
    });
}

void MainWindow::createToolBars()
{
    // Create main toolbar
    m_mainToolBar = new QToolBar(tr("Main Toolbar"), this);
    m_mainToolBar->setMovable(false);
    m_mainToolBar->setIconSize(QSize(24, 24));
    m_mainToolBar->setFloatable(false);
    
    // 移除工具栏的边框和边距
    m_mainToolBar->setStyleSheet("QToolBar { border: none; padding: 2px; }");
    
    // File operations
    m_mainToolBar->addAction(m_openLeftAction);
    m_mainToolBar->addAction(m_openRightAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_pasteLeftAction);
    m_mainToolBar->addAction(m_pasteRightAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_urlLeftAction);
    m_mainToolBar->addAction(m_urlRightAction);
    m_mainToolBar->addSeparator();
    
    // Comparison mode
    QLabel *modeLabel = new QLabel(tr("Mode:"), this);
    m_mainToolBar->addWidget(modeLabel);
    
    m_modeComboBox = new QComboBox(this);
    
    // 设置下拉箭头图标
    IconManager *iconManager = IconManager::instance();
    m_modeComboBox->setStyleSheet(QString(
        "QComboBox::down-arrow { "
        "    image: url(%1); "
        "    width: 12px; "
        "    height: 12px; "
        "}"
    ).arg(iconManager->getIconPath(IconType::ComboArrow)));
    
    m_modeComboBox->addItem(tr("Side by Side"));
    m_modeComboBox->addItem(tr("Slider Overlay"));
    m_mainToolBar->addWidget(m_modeComboBox);
}

void MainWindow::createStatusBar()
{
    m_leftImageInfoLabel = new QLabel(tr("Left: No image"));
    m_rightImageInfoLabel = new QLabel(tr("Right: No image"));
    
    statusBar()->addPermanentWidget(m_leftImageInfoLabel);
    statusBar()->addPermanentWidget(m_rightImageInfoLabel);
    
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setupLayout()
{
    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Add title bar to the layout (at the very top)
    mainLayout->addWidget(m_titleBar);
    
    // Add toolbar below the title bar
    mainLayout->addWidget(m_mainToolBar);
    
    // Create splitter for the image viewers
    m_splitter = new QSplitter(Qt::Horizontal, centralWidget);
    m_splitter->addWidget(m_leftViewer);
    m_splitter->addWidget(m_rightViewer);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 1);
    
    // Add splitter to the layout
    mainLayout->addWidget(m_splitter, 1);
    
    setCentralWidget(centralWidget);
}

void MainWindow::setupConnections()
{
    connect(m_imageLoader, &ImageLoader::imageLoaded, this, [this](const QImage &image, bool isLeft) {
        if (isLeft) {
            m_leftViewer->setImage(image);
            
            // 如果当前是叠加模式，且右侧图像已加载，则设置叠加图像
            if (m_currentMode == ComparisonMode::SliderOverlay && !m_rightViewer->getImage().isNull()) {
                m_leftViewer->setOverlayImage(m_rightViewer->getImage());
                m_leftViewer->setOverlayMode(ImageViewer::SliderOverlay);
            }
        } else {
            m_rightViewer->setImage(image);
            
            // 如果当前是叠加模式，且左侧图像已加载，则设置叠加图像
            if (m_currentMode == ComparisonMode::SliderOverlay && !m_leftViewer->getImage().isNull()) {
                m_leftViewer->setOverlayImage(image);
                m_leftViewer->setOverlayMode(ImageViewer::SliderOverlay);
            }
        }
        
        updateStatusBar();
    });
    
    connect(m_imageLoader, &ImageLoader::errorOccurred, this, [this](const QString &error) {
        statusBar()->showMessage(error, 5000);
    });
    
    // 连接文件操作信号
    connect(m_openLeftAction, &QAction::triggered, this, &MainWindow::openImageLeft);
    connect(m_openRightAction, &QAction::triggered, this, &MainWindow::openImageRight);
    connect(m_pasteLeftAction, &QAction::triggered, this, &MainWindow::pasteFromClipboardLeft);
    connect(m_pasteRightAction, &QAction::triggered, this, &MainWindow::pasteFromClipboardRight);
    connect(m_urlLeftAction, &QAction::triggered, this, &MainWindow::loadFromUrlLeft);
    connect(m_urlRightAction, &QAction::triggered, this, &MainWindow::loadFromUrlRight);
    
    // 连接比较模式切换信号
    connect(m_modeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::switchComparisonMode);
    
    // 连接主题管理器信号
    connect(ThemeManager::instance(), &ThemeManager::themeChanged, this, &MainWindow::onThemeChanged);
    
    // 连接语言管理器信号
    connect(LanguageManager::instance(), &LanguageManager::languageChanged, this, &MainWindow::onLanguageChanged);
}

void MainWindow::onThemeChanged(Theme theme)
{
    m_currentTheme = theme;
    
    // 更新主题选择状态
    m_lightThemeAction->setChecked(theme == Theme::LightTheme);
    m_darkThemeAction->setChecked(theme == Theme::DarkTheme);
    
    // 更新图标
    updateActionIcons();
    
    // 更新ComboBox下拉箭头
    IconManager *iconManager = IconManager::instance();
    m_modeComboBox->setStyleSheet(QString(
        "QComboBox::down-arrow { "
        "    image: url(%1); "
        "    width: 12px; "
        "    height: 12px; "
        "}"
    ).arg(iconManager->getIconPath(IconType::ComboArrow)));
    
    // 更新滑块颜色
    QColor sliderLineColor, sliderHandleColor;
    
    if (theme == Theme::LightTheme) {
        sliderLineColor = QColor(100, 100, 100, 200);
        sliderHandleColor = QColor(42, 130, 218, 220);
    } else {
        sliderLineColor = QColor(200, 200, 200, 200);
        sliderHandleColor = QColor(42, 130, 218, 220);
    }
    
    // 更新滑块颜色
    m_leftViewer->setSliderColor(sliderLineColor, sliderHandleColor);
    m_rightViewer->setSliderColor(sliderLineColor, sliderHandleColor);
    
    // 更新标题栏
    if (m_titleBar) {
        m_titleBar->updateStyle(theme == Theme::DarkTheme);
    }
}

void MainWindow::updateActionIcons()
{
    // 获取图标管理器实例
    IconManager *iconManager = IconManager::instance();
    
    // 更新所有操作的图标
    m_openLeftAction->setIcon(iconManager->getIcon(IconType::Open));
    m_openRightAction->setIcon(iconManager->getIcon(IconType::Open));
    m_pasteLeftAction->setIcon(iconManager->getIcon(IconType::Paste));
    m_pasteRightAction->setIcon(iconManager->getIcon(IconType::Paste));
    m_urlLeftAction->setIcon(iconManager->getIcon(IconType::Url));
    m_urlRightAction->setIcon(iconManager->getIcon(IconType::Url));
}

void MainWindow::switchComparisonMode(int index)
{
    m_currentMode = static_cast<ComparisonMode>(index);
    
    if (m_currentMode == ComparisonMode::SideBySide) {
        // 恢复到并排模式
        m_leftViewer->setOverlayMode(ImageViewer::NoOverlay);
        m_rightViewer->setVisible(true);
        m_rightViewer->setEnabled(true);
        
        // 更新布局
        if (m_splitter->count() == 1) {
            m_splitter->addWidget(m_rightViewer);
        }
        m_splitter->setSizes(QList<int>() << m_splitter->width()/2 << m_splitter->width()/2);
    } 
    else if (m_currentMode == ComparisonMode::SliderOverlay) {
        // 切换到滑块叠加模式
        QImage rightImage = m_rightViewer->getImage();
        QImage leftImage = m_leftViewer->getImage();
        
        // 设置叠加图像（无论右侧图像是否为空）
        m_leftViewer->setOverlayImage(rightImage);
        
        // 设置叠加模式
        m_leftViewer->setOverlayMode(ImageViewer::SliderOverlay);
        
        // 如果右侧图像为空，滑块不会显示（由ImageViewer内部逻辑控制）
        
        // 隐藏右侧查看器
        m_rightViewer->setVisible(false);
        m_rightViewer->setEnabled(false);
        
        // 如果右侧查看器在splitter中，则移除它
        if (m_splitter->count() > 1) {
            m_splitter->widget(1)->hide();
        }
    }
    
    // 保存当前比较模式到配置
    ConfigManager::instance()->setComparisonMode(static_cast<int>(m_currentMode));
    
    // 更新UI
    update();
}

void MainWindow::openImageLeft()
{
    // 使用上次打开的目录作为起始目录
    QString lastDir = ConfigManager::instance()->getLastOpenDirectory();
    
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Left Image"),
                                                    lastDir,
                                                    tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));
    if (!fileName.isEmpty()) {
        // 保存当前打开文件的目录
        QFileInfo fileInfo(fileName);
        ConfigManager::instance()->setLastOpenDirectory(fileInfo.absolutePath());
        
        // 加载图像
        m_imageLoader->loadFromFile(fileName, true);
    }
}

void MainWindow::openImageRight()
{
    // 使用上次打开的目录作为起始目录
    QString lastDir = ConfigManager::instance()->getLastOpenDirectory();
    
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Right Image"),
                                                    lastDir,
                                                    tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));
    if (!fileName.isEmpty()) {
        // 保存当前打开文件的目录
        QFileInfo fileInfo(fileName);
        ConfigManager::instance()->setLastOpenDirectory(fileInfo.absolutePath());
        
        // 加载图像
        m_imageLoader->loadFromFile(fileName, false);
    }
}

void MainWindow::pasteFromClipboardLeft()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    
    if (mimeData->hasImage()) {
        QImage image = qvariant_cast<QImage>(mimeData->imageData());
        m_leftViewer->setImage(image);
    } else {
        QMessageBox::warning(this, tr("Paste Error"), tr("No image found in clipboard"));
    }
}

void MainWindow::pasteFromClipboardRight()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    
    if (mimeData->hasImage()) {
        QImage image = qvariant_cast<QImage>(mimeData->imageData());
        m_rightViewer->setImage(image);
    } else {
        QMessageBox::warning(this, tr("Paste Error"), tr("No image found in clipboard"));
    }
}

void MainWindow::loadFromUrlLeft()
{
    bool ok;
    QString url = QInputDialog::getText(this, tr("Load Image from URL"),
                                         tr("Enter URL:"), QLineEdit::Normal,
                                         "https://", &ok);
    if (ok && !url.isEmpty()) {
        m_imageLoader->loadFromUrl(url, true);
    }
}

void MainWindow::loadFromUrlRight()
{
    bool ok;
    QString url = QInputDialog::getText(this, tr("Load Image from URL"),
                                         tr("Enter URL:"), QLineEdit::Normal,
                                         "https://", &ok);
    if (ok && !url.isEmpty()) {
        m_imageLoader->loadFromUrl(url, false);
    }
}

void MainWindow::updateStatusBar()
{
    QImage leftImage = m_leftViewer->getImage();
    QImage rightImage = m_rightViewer->getImage();
    
    if (!leftImage.isNull()) {
        m_leftImageInfoLabel->setText(tr("Left: %1x%2").arg(leftImage.width()).arg(leftImage.height()));
    } else {
        m_leftImageInfoLabel->setText(tr("Left: No image"));
    }
    
    if (!rightImage.isNull()) {
        m_rightImageInfoLabel->setText(tr("Right: %1x%2").arg(rightImage.width()).arg(rightImage.height()));
    } else {
        m_rightImageInfoLabel->setText(tr("Right: No image"));
    }
}

void MainWindow::saveSettings()
{
    // 保存窗口大小和位置
    if (!isMaximized()) {
        ConfigManager::instance()->setWindowSize(size());
        ConfigManager::instance()->setWindowPosition(pos());
        ConfigManager::instance()->setMaximized(false);
    } else {
        ConfigManager::instance()->setMaximized(true);
    }
    
    // 保存主题
    ConfigManager::instance()->setTheme(m_currentTheme);
    
    // 保存语言
    ConfigManager::instance()->setLanguage(m_currentLanguage);
    
    // 保存比较模式
    ConfigManager::instance()->setComparisonMode(static_cast<int>(m_currentMode));
    
    // 保存配置到文件
    ConfigManager::instance()->saveConfig();
}

void MainWindow::loadSettings()
{
    // 加载配置
    ConfigManager::instance()->loadConfig();
    
    // 应用窗口大小和位置
    if (ConfigManager::instance()->isMaximized()) {
        setWindowState(Qt::WindowMaximized);
    } else {
        resize(ConfigManager::instance()->getWindowSize());
        move(ConfigManager::instance()->getWindowPosition());
    }
    
    // 应用主题
    m_currentTheme = ConfigManager::instance()->getTheme();
    ThemeManager::instance()->applyTheme(m_currentTheme);
    
    // 应用语言
    m_currentLanguage = ConfigManager::instance()->getLanguage();
    LanguageManager::instance()->applyLanguage(m_currentLanguage);
    
    // 应用比较模式
    int modeIndex = ConfigManager::instance()->getComparisonMode();
    m_modeComboBox->setCurrentIndex(modeIndex);
    switchComparisonMode(modeIndex);
}

void MainWindow::applyTheme(Theme theme)
{
    m_currentTheme = theme;
    ThemeManager::instance()->applyTheme(theme);
}

void MainWindow::onLanguageChanged(Language language)
{
    m_currentLanguage = language;
    
    // 更新语言选择状态
    m_englishAction->setChecked(language == Language::English);
    m_chineseAction->setChecked(language == Language::Chinese);
    m_japaneseAction->setChecked(language == Language::Japanese);
    m_koreanAction->setChecked(language == Language::Korean);
    
    // 更新标题
    m_titleBar->setTitle(tr("Image Comparison Tool"));
    
    // 更新状态栏
    updateStatusBar();
    
    // 保存语言设置
    ConfigManager::instance()->setLanguage(language);
}

void MainWindow::applyLanguage(Language language)
{
    m_currentLanguage = language;
    LanguageManager::instance()->applyLanguage(language);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 保存设置
    saveSettings();
    event->accept();
}

MainWindow::~MainWindow()
{
}
