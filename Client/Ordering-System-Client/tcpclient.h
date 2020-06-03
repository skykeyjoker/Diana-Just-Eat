#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class TcpClient:public QObject
{
    Q_OBJECT
public:
    TcpClient(QObject *parent = nullptr);

    void establishConnect(QString host, int port);

    bool sendData(const QByteArray data);

    QTcpSocket *_socket = new QTcpSocket;

public slots:
    void slotReadyRead();

private:

signals:
    void signalEstablishConnect();

    void signalUpdateMenu();
};

#endif // TCPCLIENT_H
