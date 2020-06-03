#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "writejson.h"

class DialogSettings : public QDialog
{
    Q_OBJECT
public:
    explicit DialogSettings(QString dbHost, QString dbName, QString dbUser, QString dbPasswd, int dbPort, QString tcpHost, int tcpPort, QString picHost, QWidget *parent = nullptr);

    QLineEdit *le_MySqlHost;
    QLineEdit *le_MySqlPort;
    QLineEdit *le_MySqlName;
    QLineEdit *le_MySqlUser;
    QLineEdit *le_MySqlPasswd;
    QLineEdit *le_HttpHost;
    QLineEdit *le_TcpHost;
    QLineEdit *le_TcpPort;

signals:

private:
    QString _dbHost;
    QString _dbName;
    QString _dbUser;
    QString _dbPasswd;
    int _dbPort;
    QString _tcpHost;
    int _tcpPort;

    //图片服务器
    QString _picHost;

public slots:
    void slotUpdateBtnClicked();
    void slotRevBtnClicked();
};

#endif // DIALOGSETTINGS_H
