#ifndef WRITEJSON_H
#define WRITEJSON_H

#include <QtCore>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QDebug>
#include <QDir>
#include "EncryptDecrypt.h"

class WriteJson
{
public:
    WriteJson();
    WriteJson(QString dbHost, QString dbName, QString dbUser, QString dbPasswd, int dbPort, QString picHost, QString tcpHost, int tcpPort, QString tableNum)
        :_dbHost(dbHost), _dbName(dbName), _dbUser(dbUser), _dbPasswd(dbPasswd), _dbPort(dbPort), _picHost(picHost),_tcpHost(tcpHost), _tcpPort(tcpPort), _tableNum(tableNum)
    {

    }

    bool writeToFile();

private:
    //Info to json
    QString _dbHost;
    QString _dbName;
    QString _dbUser;
    QString _dbPasswd;
    int _dbPort;
    QString _picHost;
    QString _tcpHost;
    int _tcpPort;
    QString _tableNum;

};

#endif // WRITEJSON_H
