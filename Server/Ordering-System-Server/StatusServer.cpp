#include "StatusServer.h"


StatusServer::StatusServer(const int statusPort, QObject *parent)
	: m_statusPort(statusPort),
	  QObject(parent) {
}

void StatusServer::establishServer() {
	_tcpStatusServer = new QTcpServer;
	bool ret = _tcpStatusServer->listen(QHostAddress::Any, m_statusPort);

	if (!ret) {
		emit serverError();
		qDebug() << "状态服务器建立失败";
	} else
		emit serverEstablished();

	connect(_tcpStatusServer, &QTcpServer::newConnection, this, &StatusServer::slotNewStatusConnection);
}

void StatusServer::slotNewStatusConnection() {
	// 处理状态信道的新连接
	QTcpSocket *currentSocket = _tcpStatusServer->nextPendingConnection();
	_tcpStatusClients.push_back(currentSocket);

	// 向客户端发送初始化消息，启动心跳包
	using Json = nlohmann::json;
	QByteArray sendData;
	Json sendJsonData;
	sendJsonData["code"] = 2;
	sendJsonData["Msg"] = "OK";
	sendData = QString::fromStdString(sendJsonData.dump(2)).toUtf8();
	currentSocket->write(sendData);
	currentSocket->flush();
	currentSocket->waitForBytesWritten();

	connect(currentSocket, &QTcpSocket::readyRead, this, &StatusServer::slotStatusReadyRead);
	connect(currentSocket, &QTcpSocket::disconnected, [=, this]() {
		_tcpStatusClients.removeAll(currentSocket);
	});
}

void StatusServer::slotStatusReadyRead() {
	qDebug() << "心跳包thread" << this->thread();
	// 状态信道，处理客户端发来的心跳包
	QTcpSocket *currentSocket = (QTcpSocket *) sender();
	QByteArray msgData = currentSocket->readAll();

	using Json = nlohmann::json;
	Json jsonData = Json::parse(msgData.data(), nullptr, false);
	int code = jsonData["code"].get<int>();

	if (code == 2) {
		qDebug() << currentSocket << "收到一次心跳包";
		QString msg = QString::fromStdString(jsonData["Msg"].get<std::string>());
		if (msg == "Heart") {
			QByteArray sendData;
			Json sendJsonData;
			sendJsonData["code"] = 2;
			sendJsonData["Msg"] = "HEART_BACK";
			sendData = QString::fromStdString(sendJsonData.dump(2)).toUtf8();
			currentSocket->write(sendData);
			currentSocket->flush();
			currentSocket->waitForBytesWritten();
		}
	}
}
