#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent):QObject(parent)
{
    connect(_socket,&QTcpSocket::readyRead,this,&TcpClient::slotReadyRead);
}

void TcpClient::establishConnect(QString host, int port)
{
    //连接到tcp服务器
    _socket->connectToHost(host,port);

    //发送信号
    emit signalEstablishConnect();
}

void TcpClient::slotReadyRead()
{
    QByteArray data;
    data = _socket->readAll();

    QString dataStr = QString::fromUtf8(data);

    if(dataStr=="[Menu Updated]")
    {
        //TODO 发送更新菜单信号
        qDebug()<<"客户端已收到菜单更新信号";
    }
}

bool TcpClient::sendData(const QByteArray data)
{
    bool ret = _socket->write(data);

    if(!ret)
    {
        qDebug()<<"发送消息失败";
        return false;
    }

    return true;
}
