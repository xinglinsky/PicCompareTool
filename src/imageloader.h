#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

class ImageLoader : public QObject
{
    Q_OBJECT

public:
    explicit ImageLoader(QObject *parent = nullptr);
    ~ImageLoader();

    void loadFromFile(const QString &filePath, bool isLeft);
    void loadFromUrl(const QString &url, bool isLeft);

signals:
    void imageLoaded(const QImage &image, bool isLeft);
    void errorOccurred(const QString &errorMessage);

private slots:
    void handleNetworkReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    QMap<QNetworkReply*, bool> m_replyTargets; // Maps replies to isLeft flag
};

#endif // IMAGELOADER_H
