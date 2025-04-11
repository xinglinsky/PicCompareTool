#include "titlebar.h"
#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include <QStyle>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
    , m_isDarkTheme(false)
{
    setFixedHeight(32);
    setupUI();
    
    // Connect signals
    connect(m_minimizeButton, &QPushButton::clicked, this, &TitleBar::minimizeClicked);
    connect(m_maximizeButton, &QPushButton::clicked, this, &TitleBar::maximizeClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &TitleBar::closeClicked);
    connect(m_settingsButton, &QPushButton::clicked, this, &TitleBar::settingsClicked);
    
    // 设置鼠标追踪，以便在鼠标移动时更新按钮状态
    setMouseTracking(true);
}

void TitleBar::setupUI()
{
    // Create layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 0, 5, 0);
    layout->setSpacing(0);
    
    // 创建应用程序图标
    QLabel *iconLabel = new QLabel(this);
    iconLabel->setFixedSize(20, 20);
    iconLabel->setPixmap(QIcon::fromTheme("image-x-generic").pixmap(20, 20));
    layout->addWidget(iconLabel);
    layout->addSpacing(5);
    
    // Create title label
    m_titleLabel = new QLabel("Image Comparison Tool", this);
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    // Create buttons
    m_settingsButton = new QPushButton(this);
    m_settingsButton->setObjectName("settingsButton");
    m_settingsButton->setToolTip(tr("Settings"));
    m_settingsButton->setFixedSize(30, 30);
    m_settingsButton->setFlat(true);
    m_settingsButton->setCursor(Qt::PointingHandCursor);
    
    m_minimizeButton = new QPushButton(this);
    m_minimizeButton->setObjectName("minimizeButton");
    m_minimizeButton->setToolTip(tr("Minimize"));
    m_minimizeButton->setFixedSize(30, 30);
    m_minimizeButton->setFlat(true);
    m_minimizeButton->setCursor(Qt::PointingHandCursor);
    
    m_maximizeButton = new QPushButton(this);
    m_maximizeButton->setObjectName("maximizeButton");
    m_maximizeButton->setToolTip(tr("Maximize"));
    m_maximizeButton->setFixedSize(30, 30);
    m_maximizeButton->setFlat(true);
    m_maximizeButton->setCursor(Qt::PointingHandCursor);
    
    m_closeButton = new QPushButton(this);
    m_closeButton->setObjectName("closeButton");
    m_closeButton->setToolTip(tr("Close"));
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setFlat(true);
    m_closeButton->setCursor(Qt::PointingHandCursor);
    
    // Add widgets to layout
    layout->addWidget(m_titleLabel, 1);
    layout->addWidget(m_settingsButton);
    layout->addWidget(m_minimizeButton);
    layout->addWidget(m_maximizeButton);
    layout->addWidget(m_closeButton);
    
    setLayout(layout);
    
    // Set initial style
    updateStyle(false);
}

void TitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void TitleBar::updateStyle(bool isDarkTheme)
{
    m_isDarkTheme = isDarkTheme;
    
    // Set colors based on theme
    QString backgroundColor = isDarkTheme ? "#353535" : "#f0f0f0";
    QString textColor = isDarkTheme ? "#ffffff" : "#000000";
    QString buttonHoverColor = isDarkTheme ? "#454545" : "#e0e0e0";
    
    // Set stylesheet for the title bar
    setStyleSheet(QString(
        "TitleBar {"
        "  background-color: %1;"
        "  border-bottom: 1px solid %2;"
        "}"
        "QLabel {"
        "  color: %3;"
        "  font-weight: bold;"
        "}"
        "QPushButton {"
        "  border: none;"
        "  background-color: transparent;"
        "  color: %3;"
        "}"
        "QPushButton:hover {"
        "  background-color: %4;"
        "}"
        "QPushButton#closeButton:hover {"
        "  background-color: #e81123;"
        "  color: white;"
        "}"
    ).arg(backgroundColor, isDarkTheme ? "#252525" : "#d0d0d0", textColor, buttonHoverColor));
    
    // Set button icons based on theme
    m_minimizeButton->setText("🗕");
    m_maximizeButton->setText("🗖");
    m_closeButton->setText("✕");
    m_settingsButton->setText("⚙");
    
    update();
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - window()->frameGeometry().topLeft();
        event->accept();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        window()->move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit maximizeClicked();
        event->accept();
    }
}

void TitleBar::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    
    QWidget::paintEvent(event);
}
