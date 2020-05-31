#ifndef READJSON_H
#define READJSON_H

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include "EncryptDecrypt.h"

class ReadJson
{
public:
    ReadJson();

    bool readFromFile(const QString &fileName);

    QString getDbHost();
    QString getDbName();
    QString getDbUser();
    QString getDbPasswd();
    int getDbPort();
    QString getTcpHost();
    int getTcpPort();
    int getClearShot();

private:
    //Info from json
    QString _dbHost;
    QString _dbName;
    QString _dbUser;
    QString _dbPasswd;
    int _dbPort;
    QString _tcpHost;
    int _tcpPort;
    int _clearShot;
};

#endif // READJSON_H
