#include "readjson.h"

ReadJson::ReadJson()
{
}

bool ReadJson::readFromFile(const QString &fileName)
{
    QFile file(fileName); //打开文件

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "无法打开json文件";
        return false;
    }

    QByteArray data = file.readAll(); //从文件读取
    file.close();

    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(data, &jsonError)); //转换json数据

    if (jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "无法读取json";
        return false;
    }

    QJsonObject jsonObj = jsonDoc.object();

    _dbHost = jsonObj.value("dbHost").toString();
    _dbName = jsonObj.value("dbName").toString();
    _dbUser = jsonObj.value("dbUser").toString();
    _dbPasswd = jsonObj.value("dbPasswd").toString();
    _dbPort = jsonObj.value("dbPort").toInt();
    _tcpHost = jsonObj.value("tcpHost").toString();
    _tcpPort = jsonObj.value("tcpPort").toInt();
    _clearShot = jsonObj.value("clearShot").toInt();

    qDebug() << "_dbHost:" << _dbHost;
    qDebug() << "_dbName:" << _dbName;
    qDebug() << "_dbUser:" << _dbUser;
    qDebug() << "_dbPasswd:" << _dbPasswd;
    qDebug() << "_tcpHost:" << _tcpHost;
    qDebug() << "_tcpPort:" <<_tcpPort;
    qDebug() <<"_clearShot:"<<_clearShot;

    //解密
    _dbHost = XorEncryptDecrypt(_dbHost, 19);
    _dbName = XorEncryptDecrypt(_dbName, 19);
    _dbUser = XorEncryptDecrypt(_dbUser, 19);
    _dbPasswd = XorEncryptDecrypt(_dbPasswd, 19);


    qDebug() << "_dbHost:" << _dbHost;
    qDebug() << "_dbName:" << _dbName;
    qDebug() << "_dbUser:" << _dbUser;
    qDebug() << "_dbPasswd:" << _dbPasswd;
    qDebug() << "_tcpHost:" << _tcpHost;
    qDebug() << "_tcpPort:" <<_tcpPort;
    qDebug() <<"_clearShot:"<<_clearShot;


    return true;
}

QString ReadJson::getDbHost()
{
    return _dbHost;
}

QString ReadJson::getDbName()
{
    return _dbName;
}

QString ReadJson::getDbUser()
{
    return _dbUser;
}

QString ReadJson::getDbPasswd()
{
    return _dbPasswd;
}

int ReadJson::getDbPort()
{
    return _dbPort;
}

QString ReadJson::getTcpHost()
{
    return _tcpHost;
}

int ReadJson::getTcpPort()
{
    return _tcpPort;
}

int ReadJson::getClearShot()
{
    return  _clearShot;
}
