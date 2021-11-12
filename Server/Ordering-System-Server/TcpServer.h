#ifndef ORDERING_SYSTEM_TCPSERVER_H
#define ORDERING_SYSTEM_TCPSERVER_H

#include <QDebug>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "json.hpp"

class TcpServer : public QObject {
	Q_OBJECT
public:
	explicit TcpServer(const QString &tcpHost, const int tcpPort, const int tcpStatusPort, QObject *parent = 0);
	~TcpServer() noexcept;

public:
	bool establishServer();         // 建立链接
	int getCurrentClientNum() const;// 获取当前客户端数量

public:
	void sendMenu(QTcpSocket *target, const QByteArray &menu);// 向指定客户端发送菜单
	void sendUpdatedMenu(const QByteArray &menu);             // 向所有客户端发送菜单更新消息

private slots:
	void slotNewConnection();// 订单菜单信道链接
	void slotReadyRead();

	void slotNewStatusConnection();// 状态信道链接
	void slotStatusReadyRead();

signals:
	void sigQueryMenu(QTcpSocket *target);    // 客户端请求菜单
	void sigNewOrder(const QByteArray &order);// 客户端发来新菜单

private:
	// 订单菜单信道
	QTcpServer *_tcpServer;
	QList<QTcpSocket *> _tcpClients;

	// 状态信道
	QTcpServer *_tcpStatusServer;
	QList<QTcpSocket *> _tcpStatusClients;

private:
	QString m_tcpHost;
	int m_tcpPort;
	int m_tcpStatusPort;
};


#endif//ORDERING_SYSTEM_TCPSERVER_H
