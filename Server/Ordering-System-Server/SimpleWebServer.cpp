#include "SimpleWebServer.h"

SimpleWebServer::SimpleWebServer(const QString &urlDir)
	: m_urlDir(urlDir) {
	QHttpServer *server = new QHttpServer(this);
	connect(server, SIGNAL(newRequest(QHttpRequest *, QHttpResponse *)),
			this, SLOT(handleRequest(QHttpRequest *, QHttpResponse *)));

	server->listen(QHostAddress::Any, 80);
	qDebug() << this->thread()->currentThread();
}

void SimpleWebServer::handleRequest(QHttpRequest *req, QHttpResponse *resp) {
	Q_UNUSED(req);
	qDebug() << req->path();
	qDebug() << this->thread()->currentThread();

	QFile file(m_urlDir + req->path());
	QByteArray body;
	if (file.open(QFile::ReadOnly)) {
		//QByteArray body = "Hello World";
		body = file.readAll();
		resp->setHeader("Content-Length", QString::number(body.size()));
	} else {
		body = "File Open Error";
		resp->setHeader("Content-Length", QString::number(body.size()));
	}
	resp->writeHead(200);
	resp->end(body);
	file.close();
}