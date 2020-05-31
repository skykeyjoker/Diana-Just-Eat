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
    WriteJson(QString dbHost, QString dbName, QString dbUser, QString dbPasswd, int dbPort, QString tcpHost, int tcpPort, int clearShot)
        :_dbHost(dbHost), _dbName(dbName), _dbUser(dbUser), _dbPasswd(dbPasswd), _dbPort(dbPort), _tcpHost(tcpHost), _tcpPort(tcpPort),_clearShot(clearShot)
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
    QString _tcpHost;
    int _tcpPort;
    int _clearShot;
};

#endif // WRITEJSON_H
