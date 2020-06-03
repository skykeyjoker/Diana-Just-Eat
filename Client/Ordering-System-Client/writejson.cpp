#include "writejson.h"

WriteJson::WriteJson()
{
}

bool WriteJson::writeToFile()
{
    QFile file(QDir::currentPath()+"/config.json");
    if(!file.open(QIODevice::ReadWrite))
    {
        qDebug()<<"无法打开json文件";
        return false;
    }
    QJsonObject obj;
    _dbHost = XorEncryptDecrypt(_dbHost,19);
    _dbName = XorEncryptDecrypt(_dbName,19);
    _dbUser = XorEncryptDecrypt(_dbUser,19);
    _dbPasswd = XorEncryptDecrypt(_dbPasswd,19);
    _picHost =XorEncryptDecrypt(_picHost,19);

    obj.insert("dbHost",_dbHost);
    obj.insert("dbName",_dbName);
    obj.insert("dbUser",_dbUser);
    obj.insert("dbPasswd",_dbPasswd);
    obj.insert("dbPort",_dbPort);
    obj.insert("picHost",_picHost);
    obj.insert("tcpHost",_tcpHost);
    obj.insert("tcpPort",_tcpPort);


    QJsonDocument doc;
    doc.setObject(obj);

    file.write(doc.toJson());
    qDebug()<<"write to file";
    file.close();

    return true;
}
