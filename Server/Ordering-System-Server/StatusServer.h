#ifndef ORDERING_SYSTEM_STATUSSERVER_H
#define ORDERING_SYSTEM_STATUSSERVER_H

#include <QDebug>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "json.hpp"

class StatusServer : public QObject {
	Q_OBJECT
public:
	explicit StatusServer(const int statusPort, QObject *parent = nullptr);

signals:
	void serverEstablished();
	void serverError();
public slots:
	void establishServer();

private slots:
	void slotNewStatusConnection();// 状态信道链接
	void slotStatusReadyRead();

private:
	// 状态信道
	QTcpServer *_tcpStatusServer;
	QList<QTcpSocket *> _tcpStatusClients;

private:
	int m_statusPort;
};


#endif//ORDERING_SYSTEM_STATUSSERVER_H
