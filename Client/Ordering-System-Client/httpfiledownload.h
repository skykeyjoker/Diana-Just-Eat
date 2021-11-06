#ifndef HTTPFILEDOWNLOAD_H
#define HTTPFILEDOWNLOAD_H

#include <QDebug>
#include <QEventLoop>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRunnable>

class HttpFileDownload : public QRunnable {
public:
	explicit HttpFileDownload(const QString& url, const QString& fileName);
	void run() override;

private:
	QString _url;
	QString _fileName;
};

#endif// HTTPFILEDOWNLOAD_H
