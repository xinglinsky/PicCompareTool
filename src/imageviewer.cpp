#include "imageviewer.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QApplication>
#include <QScrollBar>
#include <QDebug>
#include <QPainterPath>
#include <cmath>

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent)
    , m_zoomFactor(1.0)
    , m_isDragging(false)
    , m_overlayOpacity(0.5)
    , m_overlayMode(NoOverlay)
    , m_showSlider(false)
    , m_isDraggingSlider(false)
    , m_sliderLineColor(255, 255, 255, 180)
    , m_sliderHandleColor(220, 220, 220, 220)
{
    setMinimumSize(200, 200);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);
    
    // Set a dark background to make transparent areas visible
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(50, 50, 50));
    setPalette(pal);
    setAutoFillBackground(true);
}

ImageViewer::~ImageViewer()
{
}

void ImageViewer::setImage(const QImage &image)
{
    m_image = image;
    resetView();
    emit imageChanged();
    update();
}

void ImageViewer::setOverlayImage(const QImage &image)
{
    m_overlayImage = image;
    
    // 当设置叠加图像时，只更新图像本身，不改变模式
    // 模式的切换由外部控制，这样可以更灵活地处理不同情况
    
    // 只有当基础图像和叠加图像都不为空时，才显示滑块
    m_showSlider = !m_image.isNull() && !m_overlayImage.isNull();
    
    update();
}

QImage ImageViewer::getImage() const
{
    return m_image;
}

void ImageViewer::resetView()
{
    if (m_image.isNull()) {
        m_zoomFactor = 1.0;
        m_offset = QPoint(0, 0);
        return;
    }

    // Calculate zoom factor to fit the image in the view
    double widthRatio = static_cast<double>(width()) / m_image.width();
    double heightRatio = static_cast<double>(height()) / m_image.height();
    m_zoomFactor = qMin(widthRatio, heightRatio);
    
    // Center the image
    QSize scaledSize = QSize(m_image.width() * m_zoomFactor, m_image.height() * m_zoomFactor);
    m_offset.setX((width() - scaledSize.width()) / 2);
    m_offset.setY((height() - scaledSize.height()) / 2);
    
    update();
}

void ImageViewer::zoomIn()
{
    if (m_image.isNull()) return;
    
    m_zoomFactor *= 1.1;
    updateView();
}

void ImageViewer::zoomOut()
{
    if (m_image.isNull()) return;
    
    m_zoomFactor /= 1.1;
    if (m_zoomFactor < 0.1) m_zoomFactor = 0.1;
    updateView();
}

void ImageViewer::setZoomLevel(int level)
{
    if (m_image.isNull()) return;
    
    m_zoomFactor = pow(2.0, level / 10.0);
    updateView();
}

void ImageViewer::updateView()
{
    // Ensure the image stays centered when zooming
    QSize oldSize = QSize(m_image.width() * m_zoomFactor, m_image.height() * m_zoomFactor);
    QSize newSize = QSize(m_image.width() * m_zoomFactor, m_image.height() * m_zoomFactor);
    
    m_offset.setX(m_offset.x() + (oldSize.width() - newSize.width()) / 2);
    m_offset.setY(m_offset.y() + (oldSize.height() - newSize.height()) / 2);
    
    update();
}

QPoint ImageViewer::mapToImage(const QPoint &viewportPos) const
{
    if (m_image.isNull()) return QPoint();
    
    return QPoint(
        static_cast<int>((viewportPos.x() - m_offset.x()) / m_zoomFactor),
        static_cast<int>((viewportPos.y() - m_offset.y()) / m_zoomFactor)
    );
}

QRect ImageViewer::getImageRect() const
{
    if (m_image.isNull()) return QRect();
    
    return QRect(
        m_offset.x(),
        m_offset.y(),
        static_cast<int>(m_image.width() * m_zoomFactor),
        static_cast<int>(m_image.height() * m_zoomFactor)
    );
}

void ImageViewer::setOverlayOpacity(double opacity)
{
    m_overlayOpacity = qBound(0.0, opacity, 1.0);
    update();
}

double ImageViewer::getOverlayOpacity() const
{
    return m_overlayOpacity;
}

void ImageViewer::setOverlayMode(OverlayMode mode)
{
    m_overlayMode = mode;
    
    // 只有当基础图像和叠加图像都不为空且处于SliderOverlay模式时，才显示滑块
    if (mode == SliderOverlay) {
        m_showSlider = !m_image.isNull() && !m_overlayImage.isNull();
    } else {
        m_showSlider = false;
    }
    
    update();
}

ImageViewer::OverlayMode ImageViewer::getOverlayMode() const
{
    return m_overlayMode;
}

void ImageViewer::setSliderColor(const QColor &lineColor, const QColor &handleColor)
{
    m_sliderLineColor = lineColor;
    m_sliderHandleColor = handleColor;
    update();
}

QImage ImageViewer::blendImages(const QImage &baseImage, const QImage &overlayImage, double opacity) const
{
    if (baseImage.isNull() || overlayImage.isNull()) {
        return baseImage.isNull() ? overlayImage : baseImage;
    }
    
    // Create a new image with the same size as the base image
    QImage result = baseImage.copy();
    
    // Get direct access to pixel data
    const QRgb* basePixels = reinterpret_cast<const QRgb*>(baseImage.bits());
    const QRgb* overlayPixels = reinterpret_cast<const QRgb*>(overlayImage.bits());
    QRgb* resultPixels = reinterpret_cast<QRgb*>(result.bits());
    
    int width = qMin(baseImage.width(), overlayImage.width());
    int height = qMin(baseImage.height(), overlayImage.height());
    int baseStride = baseImage.bytesPerLine() / sizeof(QRgb);
    int overlayStride = overlayImage.bytesPerLine() / sizeof(QRgb);
    int resultStride = result.bytesPerLine() / sizeof(QRgb);
    
    // Blend pixels
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb basePixel = basePixels[y * baseStride + x];
            QRgb overlayPixel = overlayPixels[y * overlayStride + x];
            
            // Extract color components
            int baseR = qRed(basePixel);
            int baseG = qGreen(basePixel);
            int baseB = qBlue(basePixel);
            int baseA = qAlpha(basePixel);
            
            int overlayR = qRed(overlayPixel);
            int overlayG = qGreen(overlayPixel);
            int overlayB = qBlue(overlayPixel);
            int overlayA = qAlpha(overlayPixel);
            
            // Blend colors based on opacity
            int resultR = qRound(baseR * (1.0 - opacity) + overlayR * opacity);
            int resultG = qRound(baseG * (1.0 - opacity) + overlayG * opacity);
            int resultB = qRound(baseB * (1.0 - opacity) + overlayB * opacity);
            int resultA = qMax(baseA, overlayA);
            
            // Store blended pixel
            resultPixels[y * resultStride + x] = qRgba(resultR, resultG, resultB, resultA);
        }
    }
    
    return result;
}

void ImageViewer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(rect(), palette().window());
    
    if (m_image.isNull()) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, tr("No Image"));
        return;
    }
    
    QRect targetRect = getImageRect();
    
    // Handle different overlay modes
    if (m_overlayMode == NoOverlay) {
        // Just draw the base image
        painter.drawImage(targetRect, m_image);
    } 
    else if (m_overlayMode == SliderOverlay) {
        // 始终绘制基础图像
        painter.drawImage(targetRect, m_image);
        
        // 只有当叠加图像存在时才绘制叠加部分
        if (!m_overlayImage.isNull()) {
            // Split view based on slider position
            double sliderPos = m_overlayOpacity;
            int splitX = targetRect.left() + qRound(targetRect.width() * sliderPos);
            
            // Create a clipping region for the right side
            QPainterPath clipPath;
            clipPath.addRect(QRect(splitX, targetRect.top(), 
                                  targetRect.right() - splitX + 1, targetRect.height()));
            painter.setClipPath(clipPath);
            
            // Draw the overlay image in the right portion
            painter.drawImage(targetRect, m_overlayImage);
            
            // Reset the clipping region
            painter.setClipping(false);
            
            // Draw the slider control
            const int sliderWidth = 4;
            const int handleRadius = 10;
            m_sliderRect = QRect(splitX - sliderWidth/2, targetRect.top(), 
                               sliderWidth, targetRect.height());
            
            // Draw slider line
            painter.setPen(Qt::NoPen);
            painter.setBrush(m_sliderLineColor);
            painter.drawRect(m_sliderRect);
            
            // Draw slider handle
            QRect handleRect(splitX - handleRadius, 
                           targetRect.top() + targetRect.height()/2 - handleRadius,
                           handleRadius * 2, handleRadius * 2);
            painter.setBrush(m_sliderHandleColor);
            painter.setPen(QPen(Qt::black, 1));
            painter.drawEllipse(handleRect);
        }
    }
    
    //// Draw a border around the image
    //painter.setOpacity(1.0);
    //painter.setPen(QPen(Qt::gray, 1));
    //painter.drawRect(targetRect);
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
    if (m_image.isNull()) return;
    
    QPoint numDegrees = event->angleDelta() / 8;
    
    if (!numDegrees.isNull()) {
        QPoint mousePos = event->position().toPoint();
        QPoint imagePos = mapToImage(mousePos);
        
        if (numDegrees.y() > 0) {
            m_zoomFactor *= 1.1;
        } else {
            m_zoomFactor /= 1.1;
            if (m_zoomFactor < 0.1) m_zoomFactor = 0.1;
        }
        
        // Adjust offset to keep the point under the mouse in the same position
        QPoint newImagePos = QPoint(
            static_cast<int>(imagePos.x() * m_zoomFactor),
            static_cast<int>(imagePos.y() * m_zoomFactor)
        );
        
        m_offset = mousePos - newImagePos;
        update();
    }
    
    event->accept();
}

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_showSlider && m_sliderRect.adjusted(-10, 0, 10, 0).contains(event->pos())) {
            m_isDraggingSlider = true;
            setCursor(Qt::SizeHorCursor);
        } else {
            m_isDragging = true;
            m_lastMousePos = event->pos();
            setCursor(Qt::ClosedHandCursor);
        }
    }
    
    QWidget::mousePressEvent(event);
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDraggingSlider) {
        QRect imageRect = getImageRect();
        if (imageRect.width() > 0) {
            double newOpacity = qBound(0.0, 
                static_cast<double>(event->pos().x() - imageRect.left()) / imageRect.width(), 
                1.0);
            setOverlayOpacity(newOpacity);
        }
    } else if (m_isDragging) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_offset += delta;
        m_lastMousePos = event->pos();
        update();
    } else if (m_showSlider && m_sliderRect.adjusted(-10, 0, 10, 0).contains(event->pos())) {
        setCursor(Qt::SizeHorCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    
    QWidget::mouseMoveEvent(event);
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        m_isDraggingSlider = false;
        
        if (m_showSlider && m_sliderRect.adjusted(-10, 0, 10, 0).contains(event->pos())) {
            setCursor(Qt::SizeHorCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
    
    QWidget::mouseReleaseEvent(event);
}

void ImageViewer::resizeEvent(QResizeEvent *event)
{
    if (!m_image.isNull()) {
        // Recalculate zoom factor to fit the image in the new view size
        double widthRatio = static_cast<double>(width()) / m_image.width();
        double heightRatio = static_cast<double>(height()) / m_image.height();
        double newZoomFactor = qMin(widthRatio, heightRatio);
        
        // Only auto-fit if the image is larger than the view or if it's too small
        if (m_zoomFactor < 0.1 || (m_image.width() > width() || m_image.height() > height())) {
            m_zoomFactor = newZoomFactor;
        }
        
        // Maintain center when resizing
        QPoint center = QPoint(width() / 2, height() / 2);
        QPoint imageCenter = QPoint(
            static_cast<int>(m_image.width() * m_zoomFactor / 2),
            static_cast<int>(m_image.height() * m_zoomFactor / 2)
        );
        m_offset = center - imageCenter;
        
        update();
    }
    
    QWidget::resizeEvent(event);
}
