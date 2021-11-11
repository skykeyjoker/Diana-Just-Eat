#ifndef ORDERING_SYSTEM_SIMPLEWEBSERVER_H
#define ORDERING_SYSTEM_SIMPLEWEBSERVER_H

#include <QDebug>
#include <QFile>
#include <QObject>
#include <QThread>

#include "qhttpserver/qhttprequest.h"
#include "qhttpserver/qhttpresponse.h"
#include "qhttpserver/qhttpserver.h"

class SimpleWebServer : public QObject {
	Q_OBJECT
public:
	SimpleWebServer(const QString &urlDir);
	~SimpleWebServer() = default;
private slots:
	void handleRequest(QHttpRequest *req, QHttpResponse *resp);

private:
	QString m_urlDir;
};


#endif//ORDERING_SYSTEM_SIMPLEWEBSERVER_H
