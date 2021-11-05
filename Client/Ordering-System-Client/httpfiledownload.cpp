#include "httpfiledownload.h"

HttpFileDownload::HttpFileDownload(const QString &url, const QString &fileName)
	: _url(url),
	  _fileName(fileName) {
}

void HttpFileDownload::run() {
	QNetworkAccessManager manager;
	QNetworkRequest request;
	request.setUrl(QUrl(_url));

	QEventLoop eventLoop;
	QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
	QNetworkReply *reply = manager.get(request);
	eventLoop.exec();

	if (reply->error() != QNetworkReply::NoError) {
		qDebug() << "Download Failed at {}\n"
				 << _url;
	}

	QFile file(_fileName);
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "Failed to write " << _fileName;
	}
	file.write(reply->readAll());
	file.flush();
	file.close();
	qDebug() << "Download compeleted: " << _fileName;
}