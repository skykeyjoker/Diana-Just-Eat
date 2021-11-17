#include "tcpclient.h"

TcpClient::TcpClient(const QString &host, int port, int statusPort, QObject *parent)
	: _host(host),
	  _port(port),
	  _statusPort(statusPort),
	  QObject(parent) {

	_socket = new QTcpSocket;

	_statusSocket = new StatusClient(host, statusPort);
	_statusClientThread = new QThread;
	_statusSocket->moveToThread(_statusClientThread);

	qDebug() << "Tcp Client Main Thread" << this->thread();
	connect(_statusClientThread, &QThread::started, _statusSocket, &StatusClient::connectedToServer);
	connect(_statusClientThread, &QThread::finished, _statusSocket, &StatusClient::deleteLater);
}

TcpClient::~TcpClient() noexcept {
	_statusClientThread->terminate();
}

bool TcpClient::establishConnect() {
	connect(_socket, &QTcpSocket::readyRead, this, &TcpClient::slotReadyRead);

	// 断连
	connect(_socket, &QTcpSocket::disconnected, this, &TcpClient::signalDisconnectedToServer);
	connect(_statusSocket, &StatusClient::signalDisconnectedToServer, this, &TcpClient::signalDisconnectedToServer);

	//连接到tcp服务器
	_socket->connectToHost(_host, _port);
	_statusClientThread->start();

	if (!_socket->waitForConnected()) {
		return false;
	}

	return true;
}

void TcpClient::slotReadyRead() {
	QByteArray data;
	data = _socket->readAll();

	QString dataStr = QString::fromUtf8(data);
	Json json = Json::parse(dataStr.toUtf8(), nullptr, false);
	int code = json["code"].get<int>();

	switch (code) {
		case 0: {
			// 菜单请求返回处理
			emit signalQueryMenu(dataStr.toUtf8());
			break;
		}
		case 1: {
			// 菜单更新消息
			emit signalUpdateMenu(dataStr.toUtf8());
			break;
		}
		default:
			break;
	}
}

bool TcpClient::sendData(const QByteArray &data) {
	// 订单菜单信道
	bool ret = _socket->write(data);
	//_socket->flush();
	_socket->waitForBytesWritten();

	if (!ret) {
		qDebug() << "发送消息失败";
		return false;
	}

	return true;
}

void TcpClient::queryMenu() {
	// 向服务端发出消息请求（菜单订单信道）
	QString tstr = "{\"code\":0}";
	sendData(tstr.toUtf8());
}

bool TcpClient::sendNewOrder(const QByteArray &data) {
	// 信道0（菜单订单信道发送菜单消息）
	return sendData(data);
}
