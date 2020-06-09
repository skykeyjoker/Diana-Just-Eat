#include "httpfileload.h"

HttpFileLoad::HttpFileLoad(QString filePath, QString url, QObject *parent) : QObject(parent)
{
    _filePath = filePath;
    _filePath.replace("\\","/");
    qDebug()<<"_filePath"<<_filePath;
    _url =url;
    qDebug()<<_url;

    QFileInfo info(filePath);
    _fileName = info.fileName();
    _fileSuffix = info.suffix();
    qDebug()<<_fileSuffix;

}

bool HttpFileLoad::upload()
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;

    if(_fileSuffix=="jpeg")
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    if(_fileSuffix=="jpg")
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpg"));
    if(_fileSuffix=="png")
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));

    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(tr("form-data; name=\"file\"; filename=\"%1\"").arg(_fileName)));

    file = new QFile(_filePath);

    if(file->open(QIODevice::ReadWrite)==false)
    {
        qDebug()<<"文件打开失败";
    }
    else
    {
        qDebug()<<"图片打开成功";
    }
    imagePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(imagePart);

    //QUrl url("http://pic.skykeyjoker.xyz/upload_file.php");
    QUrl url(_url);
    QNetworkRequest request(url);

    uploadManager = new QNetworkAccessManager;
    uploadReply = uploadManager->post(request, multiPart);
    qDebug()<<"post send";
    multiPart->setParent(uploadReply);


    if(uploadReply->error()==QNetworkReply::NoError)
    {
        qDebug()<<"upload file finished";

        return true;
    }
    else
    {
        qDebug()<<"error string:"<<uploadReply->errorString();
        return false;
    }

    return true;
}

QString HttpFileLoad::getFileName()
{
    return _fileName;
}

