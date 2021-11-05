#ifndef HTTPFILEDOWNLOAD_H
#define HTTPFILEDOWNLOAD_H

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QPixmap>
#include <QUrl>

// TODO 改进HTTP文件下载
// TODO 重写下载worker，继承自QRunnable

class HttpFileDownload : public QObject {
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
	void updateDataReadProgress(qint64, qint64);
};

#endif// HTTPFILEDOWNLOAD_H
