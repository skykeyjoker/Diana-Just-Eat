#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent):QObject(parent)
{

}

void TcpClient::establishConnect(QString host, int port)
{
    //连接到tcp服务器
    _socket->connectToHost(host,port);

    //发送信号
    emit signalEstablishConnect();
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
