#include "StatusClient.h"

StatusClient::StatusClient(const QString host, const int port, QObject *parent)
	: m_host(host),
	  m_port(port),
	  QObject(parent) {
}

void StatusClient::connectedToServer() {
	_statusSocket = new QTcpSocket;

	connect(_statusSocket, &QTcpSocket::readyRead, this, &StatusClient::slotStatusReadyRead);
	connect(_statusSocket, &QTcpSocket::disconnected, this, &StatusClient::signalDisconnectedToServer);

	_statusSocket->connectToHost(m_host, m_port);

	if (!_statusSocket->waitForConnected()) {
		emit signalDisconnectedToServer();
	}
}

void StatusClient::slotStatusReadyRead() {
	// 收到服务端发来的客户端状态查询
	QByteArray data;
	data = _statusSocket->readAll();

	using Json = nlohmann::json;
	// 判断是否为状态查询
	QString dataStr = QString::fromUtf8(data);
	Json json = Json::parse(dataStr.toUtf8(), nullptr, false);
	int code = json["code"].get<int>();

	if (code == 2) {
		QString msgStr = QString::fromStdString(json["Msg"].get<std::string>());

		if (msgStr == "OK")// 服务器返回"OK"，开启心跳包检测
		{
			heart = new TcpHeart;
			// 开始心跳检测
			heart->startHeartTimer();
			connect(heart, &TcpHeart::sigHeartReq, this, &StatusClient::slotWriteHeartSocket);// 发送心跳包
			connect(this, &StatusClient::sigHeartBack, heart, &TcpHeart::slotHeartBack);

			// 处理心跳包异常
			connect(heart, &TcpHeart::sigHeartBad, this, &StatusClient::slotHeartBad);
		}

		// 心跳反馈
		if (msgStr == "HEART_BACK") {
			qDebug() << "HEART BACK ONCE";
			emit sigHeartBack();
		}
	}
}

void StatusClient::slotWriteHeartSocket() {
	QByteArray data;
	// 构建状态返回
	using Json = nlohmann::json;
	Json jsonValue;
	jsonValue["code"] = 2;
	jsonValue["Msg"] = "Heart";
	data = QString::fromStdString(jsonValue.dump(2)).toUtf8();

	// 状态信道，向服务端发送心跳包
	qDebug() << this->thread() << "发送一次心跳包";

	// 状态信道
	bool ret = _statusSocket->write(data);
	_statusSocket->waitForBytesWritten();

	if (!ret) {
		qDebug() << "发送心跳包失败";
		emit signalDisconnectedToServer();
	}
}

void StatusClient::slotHeartBad() {
	// 断线处理
	qDebug() << "心跳包断线";
	_statusSocket->disconnectFromHost();
	emit signalDisconnectedToServer();
}
