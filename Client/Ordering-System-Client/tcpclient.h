#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

#include "TcpHeart.h"
#include "json.hpp"

using Json = nlohmann::json;

class TcpClient : public QObject {
	Q_OBJECT
public:
	explicit TcpClient(const QString &host, int port, int statusPort, QObject *parent = nullptr);

public slots:
	void establishConnect();

public:
	// 对外进提供封装好的订单信息发送、回复服务器、请求订单方法
	void queryMenu();
	bool sendNewOrder(const QByteArray &data);

private:
	// senData函数负责具体信息发送
	bool sendData(const int signal, const QByteArray &data);

private:
	// 初始化放在构造函数内
	// 两条信道
	QTcpSocket *_socket;// 菜单订单信道

	// 客户端状态信道
	QTcpSocket *_statusSocket;// 客户端状态信道
	TcpHeart *heart;

private:
	QString _host;
	int _port;
	int _statusPort;

private slots:
	void slotReadyRead();       // 菜单订单信道
	void slotStatusReadyRead(); // 客户端状态信道
	void slotWriteHeartSocket();// 写心跳包
	void slotHeartBad();        // 心跳包掉线处理

signals:
	void signalEstablishConnect();

	void signalQueryMenu(const QByteArray data);

	void signalUpdateMenu(const QByteArray data);

	void signalDisconnectedToServer();

	void sigHeartBack();// 心跳包返回
};

#endif// TCPCLIENT_H
