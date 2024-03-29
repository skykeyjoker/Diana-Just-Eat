#include "TcpServer.h"

TcpServer::TcpServer(const QString &tcpHost, const int tcpPort, const int tcpStatusPort, QObject *parent)
	: m_tcpHost(tcpHost),
	  m_tcpPort(tcpPort),
	  m_tcpStatusPort(tcpStatusPort),
	  QObject(parent) {

	_tcpServer = new QTcpServer;

	statusThread = new QThread;
	_tcpStatusServer = new StatusServer(tcpStatusPort);
	_tcpStatusServer->moveToThread(statusThread);

	qDebug() << "Tcp Server Main Thread" << this->thread();
	connect(statusThread, &QThread::started, _tcpStatusServer, &StatusServer::establishServer);
	connect(statusThread, &QThread::finished, _tcpStatusServer, &StatusServer::deleteLater);
}

TcpServer::~TcpServer() noexcept {
	statusThread->terminate();
}

bool TcpServer::establishServer() {
	bool tcpRet = _tcpServer->listen(QHostAddress::Any, m_tcpPort);

	statusThread->start();

	connect(_tcpStatusServer, &StatusServer::serverError, this, &TcpServer::sigStatusServerError);

	if (!tcpRet) {
		return false;
	} else
		qDebug() << "成功建立Tcp服务端";

	// 处理对应信号槽
	connect(_tcpServer, &QTcpServer::newConnection, this, &TcpServer::slotNewConnection);

	return true;
}

int TcpServer::getCurrentClientNum() const {
	return _tcpClients.size();
}

void TcpServer::slotNewConnection() {
	// 处理订单菜单信道的新连接
	QTcpSocket *currentSocket = _tcpServer->nextPendingConnection();
	_tcpClients.push_back(currentSocket);

	connect(currentSocket, &QTcpSocket::readyRead, this, &TcpServer::slotReadyRead);
	connect(currentSocket, &QTcpSocket::disconnected, [=, this]() {
		_tcpClients.removeAll(currentSocket);
	});
}

void TcpServer::slotReadyRead() {
	// 菜单订单信道收到消息
	QTcpSocket *currentSocket = (QTcpSocket *) sender();
	QByteArray msgData = currentSocket->readAll();
	qDebug() << "菜单订单信道收到消息" << currentSocket << msgData;

	using Json = nlohmann::json;
	Json jsonData = Json::parse(msgData.data(), nullptr, false);
	int code = jsonData["code"].get<int>();

	switch (code) {
		case 0: {
			// 请求菜单
			qDebug() << "客户端请求新订单";
			emit sigQueryMenu(currentSocket);
			break;
		}
		case 1: {
			// 新订单
			qDebug() << currentSocket << "客户端发来新订单";
			emit sigNewOrder(msgData);
			break;
		}
		default:
			break;
	}
}

void TcpServer::sendMenu(QTcpSocket *target, const QByteArray &menu) {
	// 向指定客户端发送菜单消息
	qDebug() << target << "发送菜单";
	target->write(menu);
	target->flush();
	target->waitForBytesWritten();
}

void TcpServer::sendUpdatedMenu(const QByteArray &menu) {
	// 向所有客户端发送菜单更新消息
	for (auto currentSocket : _tcpClients) {
		qDebug() << currentSocket << "发送菜单更新消息";
		currentSocket->write(menu);
		currentSocket->flush();
		currentSocket->waitForBytesWritten();
	}
}