#include "imageloader.h"
#include <QFileInfo>
#include <QImageReader>
#include <QBuffer>

ImageLoader::ImageLoader(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_networkManager, &QNetworkAccessManager::finished, 
            this, &ImageLoader::handleNetworkReply);
}

ImageLoader::~ImageLoader()
{
}

void ImageLoader::loadFromFile(const QString &filePath, bool isLeft)
{
    QImageReader reader(filePath);
    QImage image = reader.read();
    
    if (image.isNull()) {
        emit errorOccurred(tr("Failed to load image from file: %1\nError: %2")
                          .arg(filePath)
                          .arg(reader.errorString()));
        return;
    }
    
    emit imageLoaded(image, isLeft);
}

void ImageLoader::loadFromUrl(const QString &url, bool isLeft)
{
    QUrl qUrl(url);
    if (!qUrl.isValid()) {
        emit errorOccurred(tr("Invalid URL: %1").arg(url));
        return;
    }
    
    QNetworkRequest request(qUrl);
    QNetworkReply *reply = m_networkManager->get(request);
    m_replyTargets[reply] = isLeft;
}

void ImageLoader::handleNetworkReply(QNetworkReply *reply)
{
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(tr("Network error: %1").arg(reply->errorString()));
        m_replyTargets.remove(reply);
        return;
    }
    
    QByteArray imageData = reply->readAll();
    QImage image;
    if (!image.loadFromData(imageData)) {
        emit errorOccurred(tr("Failed to load image from data"));
        m_replyTargets.remove(reply);
        return;
    }
    
    bool isLeft = m_replyTargets.value(reply);
    m_replyTargets.remove(reply);
    
    emit imageLoaded(image, isLeft);
}
