#ifndef TCPCLIENT_H
#define TCPCLIENT_H

// TODO 架构更新后TCP通信大改

#include <QObject>
#include <QTcpSocket>

#include "json.hpp"

using Json = nlohmann::json;

class TcpClient : public QObject {
	Q_OBJECT
public:
	explicit TcpClient(QObject *parent = nullptr);

	void establishConnect(const QString &host, int port, int statusPort);

public:
	// TODO 对外进提供封装好的订单信息发送、回复服务器、请求订单方法
	void queryMenu();
	bool sendNewOrder(const QByteArray &data);
	void replyStatusCheck();

private:
	// TODO senData函数底层抽象
	bool sendData(const int signal, const QByteArray &data);

private:
	// TODO 初始化放在构造函数内
	// 两条信道
	QTcpSocket *_socket;// 菜单订单信道

	// TODO 客户端状态信道
	QTcpSocket *_statusSocket;// 客户端状态信道

public slots:
	void slotReadyRead();      // 菜单订单信道
	void slotStatusReadyRead();// 客户端状态信道

signals:
	void signalEstablishConnect();

	void signalQueryMenu(const QByteArray data);

	void signalUpdateMenu(const QByteArray data);

	void signalDisconnectedToServer();
};

#endif// TCPCLIENT_H
