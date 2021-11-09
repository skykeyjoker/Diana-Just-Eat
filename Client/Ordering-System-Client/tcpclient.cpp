#include "tcpclient.h"

TcpClient::TcpClient(const QString &host, int port, int statusPort, QObject *parent)
	: _host(host),
	  _port(port),
	  _statusPort(statusPort),
	  QObject(parent) {
}

void TcpClient::establishConnect() {
	_socket = new QTcpSocket;
	_statusSocket = new QTcpSocket;
	connect(_socket, &QTcpSocket::readyRead, this, &TcpClient::slotReadyRead);
	connect(_statusSocket, &QTcpSocket::readyRead, this, &TcpClient::slotStatusReadyRead);

	// 断连
	connect(_socket, &QTcpSocket::disconnected, this, &TcpClient::signalDisconnectedToServer);
	connect(_statusSocket, &QTcpSocket::disconnected, this, &TcpClient::signalDisconnectedToServer);

	//连接到tcp服务器
	_socket->connectToHost(_host, _port);
	_statusSocket->connectToHost(_host, _statusPort);

	//发送信号
	emit signalEstablishConnect();
}

void TcpClient::slotStatusReadyRead() {
	// 收到服务端发来的客户端状态查询
	QByteArray data;
	data = _statusSocket->readAll();

	// 判断是否为状态查询
	// 立即返回存活确认

	// 服务端发送后，waitForReadAvailable，阻塞等待活跃查询
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

bool TcpClient::sendData(const int signal, const QByteArray &data) {
	// 双信道发送
	switch (signal) {
		case 0: {
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
		case 1: {
			// 状态信道
			break;
		}
		default:
			break;
	}
}

void TcpClient::queryMenu() {
	// TODO 向服务端发出消息请求（菜单订单信道），并将菜单消息返回
	QString tstr = "{\"code\":0}";
	sendData(0, tstr.toUtf8());
}

bool TcpClient::sendNewOrder(const QByteArray &data) {
	// 信道0（菜单订单信道发送菜单消息）
	return sendData(0, data);
}

void TcpClient::replyStatusCheck() {
	// TODO 返回服务端的请求状态查询
}
