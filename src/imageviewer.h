#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QPoint>
#include <QScrollArea>
#include <QLabel>
#include <QSlider>
#include <QColor>

enum class ComparisonMode { SideBySide, SliderOverlay };

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer();

    enum OverlayMode {
        NoOverlay,
        SliderOverlay
    };

    void setImage(const QImage &image);
    void setOverlayImage(const QImage &image);
    QImage getImage() const;
    void resetView();
    void zoomIn();
    void zoomOut();
    void setZoomLevel(int level);
    void setOverlayOpacity(double opacity);
    double getOverlayOpacity() const;
    
    void setOverlayMode(OverlayMode mode);
    OverlayMode getOverlayMode() const;
    
    // Set the slider appearance
    void setSliderColor(const QColor &lineColor, const QColor &handleColor);

signals:
    void imageChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateView();
    QPoint mapToImage(const QPoint &viewportPos) const;
    QRect getImageRect() const;
    QImage blendImages(const QImage &baseImage, const QImage &overlayImage, double opacity) const;

    QImage m_image;
    QImage m_overlayImage;
    double m_zoomFactor;
    QPoint m_offset;
    QPoint m_lastMousePos;
    bool m_isDragging;
    double m_overlayOpacity;
    OverlayMode m_overlayMode;
    bool m_showSlider;
    QRect m_sliderRect;
    bool m_isDraggingSlider;
    QColor m_sliderLineColor;
    QColor m_sliderHandleColor;
};

#endif // IMAGEVIEWER_H
