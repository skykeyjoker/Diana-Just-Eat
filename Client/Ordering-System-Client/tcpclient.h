#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QDebug>
#include <QObject>
#include <QTcpSocket>
#include <QThread>

#include "StatusClient.h"
#include "json.hpp"

using Json = nlohmann::json;

class TcpClient : public QObject {
	Q_OBJECT
public:
	explicit TcpClient(const QString &host, int port, int statusPort, QObject *parent = nullptr);
	~TcpClient() noexcept;

public:
	bool establishConnect();

public:
	// 对外进提供封装好的订单信息发送、回复服务器、请求订单方法
	void queryMenu();
	bool sendNewOrder(const QByteArray &data);

private:
	// senData函数负责具体信息发送
	bool sendData(const QByteArray &data);

private:
	// 初始化放在构造函数内
	// 两条信道
	QTcpSocket *_socket;// 菜单订单信道

	// 客户端状态信道
	StatusClient *_statusSocket;
	QThread *_statusClientThread;

private:
	QString _host;
	int _port;
	int _statusPort;

private slots:
	void slotReadyRead();// 菜单订单信道

signals:
	void signalQueryMenu(const QByteArray data);

	void signalUpdateMenu(const QByteArray data);

	void signalDisconnectedToServer();
};

#endif// TCPCLIENT_H
