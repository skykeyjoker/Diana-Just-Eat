#ifndef HTTPFILEDOWNLOAD_H
#define HTTPFILEDOWNLOAD_H

#include <QDebug>
#include <QEventLoop>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRunnable>

// TODO 改进HTTP文件下载
// TODO 重写下载worker，继承自QRunnable

class HttpFileDownload : public QRunnable {
public:
	explicit HttpFileDownload(const QString& url, const QString& fileName);
	void run() override;

private:
	QString _url;
	QString _fileName;
};

#endif// HTTPFILEDOWNLOAD_H
