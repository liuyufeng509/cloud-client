#include "httppicdownload.h"
#include <QApplication>
#include<QMessageBox>
HttpPicDownLoad::HttpPicDownLoad(QObject *parent) : QObject(parent)
{
    Init();
}

void HttpPicDownLoad::Init()
{
    manager = new QNetworkAccessManager(this);
}

void HttpPicDownLoad::downLoadPic()
{
    if(urls.size()==0)
    {
        //QMessageBox::information(NULL,tr("提示"),tr("请设置图片下载路径"));
        return;
    }else
        downLoadPic(urls.takeFirst());
}

void HttpPicDownLoad::doProcessReadyRead()                 //读取储存图片
{
    QByteArray tmp = reply->readAll();
    str.append(tmp);

    //qDebug()<<"read ready:"<<tmp.toHex(' ');
}

void HttpPicDownLoad::downLoadPic(QString url_str)
{
    str.clear();
    QNetworkRequest requst;
    requst.setRawHeader(QByteArray("User-Agent"), QByteArray("Sun, 06 Nov 1994 08:49:37 GMT"));   //设置头
    QUrl url;
    url.setUrl(url_str);
    requst.setUrl(url);
    reply = manager->get(requst);      //发送请求

    connect(reply, &QIODevice::readyRead, this, &HttpPicDownLoad::doProcessReadyRead,Qt::DirectConnection);            //收到可读信号
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),    //收到异常信号
          [=](QNetworkReply::NetworkError code)
            {
                switch((int)code)
                {
                    case QNetworkReply::ConnectionRefusedError:
                        qDebug() << "远程服务器拒绝连接（服务器不接受请求）";
                        break;
                    case QNetworkReply::HostNotFoundError:
                        qDebug() << "找不到远程主机名（无效的主机名）";
                        break;
                    case QNetworkReply::TimeoutError:
                        qDebug() << "与远程服务器的连接超时";
                        break;
                    default:
                        break;
                }
            });
    connect(reply, &QNetworkReply::finished,this,&HttpPicDownLoad::doProceesFinished,Qt::DirectConnection);
}
#include <QFile>
void HttpPicDownLoad::doProceesFinished()
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug()<<"some error happens when downloading pics";
        return;
    }
    //pixmap.loadFromData(str);
    QString path = qApp->applicationDirPath()+"/image/"+QString::number(counter)+".jpg";
    //pixmap.save(path, "jpg");

        QFile file(path);
      if (!file.open(QIODevice::WriteOnly|QIODevice::Append))
      {
          qDebug() <<"打开失败";
          return;
      }
      else
      {
          file.write(str);
          file.close();
      }


    counter++;
    emit alreadyDownload(path,counter);
    qDebug()<<"path:"<<path<<" counter:"<<counter;
    str.clear();
    qDebug() <<"结束";
    downLoadPic();
}

