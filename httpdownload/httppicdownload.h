#ifndef HTTPPICDOWNLOAD_H
#define HTTPPICDOWNLOAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPixmap>

class HttpPicDownLoad : public QObject
{
    Q_OBJECT
public:
    explicit HttpPicDownLoad(QObject *parent = nullptr);

    void Init();

    void downLoadPic(QString url);
    void downLoadPic();
    void addUrl(QString url){urls.append(url);}
private:
    QNetworkAccessManager *manager;
    QNetworkReply  *reply;
    QPixmap pixmap;
    int counter = 0;

    QByteArray str;//接收图片
    QStringList urls;//所有图片的路径
signals:
    void alreadyDownload(QString path,int counter);
public slots:
    void doProcessReadyRead();
    void doProceesFinished();
};

#endif // HTTPPICDOWNLOAD_H
