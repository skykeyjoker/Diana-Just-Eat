#ifndef HTTPFILEDOWNLOAD_H
#define HTTPFILEDOWNLOAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QDir>


class HttpFileDownload : public QObject
{
    Q_OBJECT
public:
    explicit HttpFileDownload(QString url, QString fileName, QObject *parent = nullptr);

    QNetworkAccessManager netManager;
    QFile *file;
    QNetworkReply *reply;
    QString _url;
    QString _fileName;

signals:
    void signalDownloadFinished();

public slots:
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64,qint64);


};

#endif // HTTPFILEDOWNLOAD_H
