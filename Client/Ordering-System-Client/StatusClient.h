#ifndef ORDERING_SYSTEM_STATUSCLIENT_H
#define ORDERING_SYSTEM_STATUSCLIENT_H

#include <QDebug>
#include <QObject>
#include <QTcpSocket>

#include "TcpHeart.h"
#include "json.hpp"

class StatusClient : public QObject {
	Q_OBJECT
public:
	explicit StatusClient(const QString host, const int port, QObject *parent = nullptr);

public slots:
	void connectedToServer();

signals:
	void sigHeartBack();              // 心跳包返回
	void signalDisconnectedToServer();//  心跳包断连
private slots:
	void slotStatusReadyRead(); // 客户端状态信道
	void slotWriteHeartSocket();// 写心跳包
	void slotHeartBad();        // 心跳包掉线处理

private:
	// 客户端状态信道
	QTcpSocket *_statusSocket;// 客户端状态信道
	TcpHeart *heart;

private:
	QString m_host;
	int m_port;
};


#endif//ORDERING_SYSTEM_STATUSCLIENT_H
