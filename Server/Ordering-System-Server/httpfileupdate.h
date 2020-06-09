#ifndef HTTPFILEUPDATE_H
#define HTTPFILEUPDATE_H


#include <QObject>
#include <QWidget>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QFileDialog>


class HttpFileUpdate : public QObject
{
    Q_OBJECT
public:
    explicit HttpFileUpdate(QString fileName, QString url, QObject *parent = nullptr);

    void update();

    void updateFinished();

    QNetworkReply *uploadReply;
    QNetworkAccessManager *m_httpNAM;

signals:


private:
    QString _url;
    QString _fileName;

};

#endif // HTTPFILEUPDATE_H
