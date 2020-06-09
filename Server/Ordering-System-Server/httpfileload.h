#ifndef HTTPFILELOAD_H
#define HTTPFILELOAD_H


#include <QObject>
#include <QFileInfo>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QIODevice>
#include <QWidget>


class HttpFileLoad : public QObject
{
    Q_OBJECT
public:
    explicit HttpFileLoad(QString filePath, QString url, QObject *parent = nullptr);

    bool upload();

    QString getFileName();

    QFile *file;

    QNetworkAccessManager *uploadManager;
    QNetworkReply *uploadReply;

signals:

private:
    QString _filePath;
    QString _fileName;
    QString _fileSuffix;
    QString _url;

};

#endif // HTTPFILELOAD_H
