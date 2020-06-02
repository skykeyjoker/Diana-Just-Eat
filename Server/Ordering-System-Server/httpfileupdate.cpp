#include "httpfileupdate.h"

HttpFileUpdate::HttpFileUpdate(QString fileName, QString url, QObject *parent) : QObject(parent)
{
    _fileName=fileName;
    _url=url;

}

void HttpFileUpdate::update()
{

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart text_part;
    text_part.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    text_part.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data;name=\"file_name\""));
    //text_part.setBody("扬州炒饭.jpg");
    text_part.setBody(_fileName.toUtf8());
    //qDebug()<<QUrl::toPercentEncoding(_fileName,".jpg|.jpeg|.png");

    multiPart->append(text_part);


    //QUrl url("http://pic.skykeyjoker.xyz/update.php");
    QUrl url(_url);
    QNetworkRequest request(url);

    m_httpNAM = new QNetworkAccessManager;
    uploadReply = m_httpNAM->post(request, multiPart);
    qDebug()<<"post send";
    multiPart->setParent(uploadReply);
    connect(uploadReply,&QNetworkReply::finished,this,&HttpFileUpdate::updateFinished);
}

void HttpFileUpdate::updateFinished()
{
    if(uploadReply->error()==QNetworkReply::NoError)
    {
        qDebug()<<"update finished";

    }
    else
    {
        qDebug()<<"error string:"<<uploadReply->errorString();
    }
}
