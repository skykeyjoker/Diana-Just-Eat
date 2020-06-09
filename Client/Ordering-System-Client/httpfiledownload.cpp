#include "httpfiledownload.h"

HttpFileDownload::HttpFileDownload(QString url, QString fileName, QObject *parent)
    : QObject(parent),_url(url), _fileName(fileName)
{
    QUrl dUrl(_url);

    file = new QFile(QDir::currentPath()+"/Pic/"+_fileName);
    if(!file->open(QIODevice::WriteOnly))
    {
        qDebug()<<"文件打开失败";
    }

    QNetworkRequest request;
    request.setUrl(url);
    reply = netManager.get(request);

    connect(reply,SIGNAL(finished()),this,SLOT(httpFinished()));//下载完成后
    connect(reply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));//有可用数据时
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateDataReadProgress(qint64,qint64))); //打印下载进度
}

void HttpFileDownload::httpReadyRead()
{
    if(file)
        file->write(reply->readAll());
}

void HttpFileDownload::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    qDebug()<<bytesRead<<":"<<totalBytes;
}

void HttpFileDownload::httpFinished()
{
    file->flush();
    file->close();
    emit signalDownloadFinished();

    reply->deleteLater();
    reply=0;
    delete file;
    file = 0;
}
