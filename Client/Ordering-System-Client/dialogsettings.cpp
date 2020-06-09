#include "dialogsettings.h"

DialogSettings::DialogSettings(QString dbHost, QString dbName, QString dbUser, QString dbPasswd, int dbPort, QString tcpHost, int tcpPort, QString picHost, QString tableNum,QWidget *parent)
    : QDialog(parent), _dbHost(dbHost), _dbName(dbName), _dbUser(dbUser), _dbPasswd(dbPasswd), _dbPort(dbPort), _tcpHost(tcpHost), _tcpPort(tcpPort), _picHost(picHost), _tableNum(tableNum)
{
    this->setWindowTitle("客户端设置");
    this->setWindowIcon(QIcon(":/Res/settings.png"));


    QVBoxLayout *layConfig = new QVBoxLayout(this);

    QGroupBox *groupMySql = new QGroupBox("数据库设置");
    QVBoxLayout *layMySql = new QVBoxLayout(groupMySql);
    QLabel *lb_MySqlHost = new QLabel("MySql服务器地址：");
    le_MySqlHost = new QLineEdit;
    QLabel *lb_MySqlPort = new QLabel("MySql服务器端口：");
    le_MySqlPort = new QLineEdit;
    QLabel *lb_MySqlName = new QLabel("数据库名称：");
    le_MySqlName = new QLineEdit;
    QLabel *lb_MySqlUser = new QLabel("数据库用户名：");
    le_MySqlUser = new QLineEdit;
    QLabel *lb_MySqlPasswd = new QLabel("数据库密码：");
    le_MySqlPasswd = new QLineEdit;
    QHBoxLayout *layMySqlHost = new QHBoxLayout;
    layMySqlHost->addWidget(lb_MySqlHost);
    layMySqlHost->addWidget(le_MySqlHost);
    layMySqlHost->addStretch(1);
    QHBoxLayout *layMySqlPort = new QHBoxLayout;
    layMySqlPort->addWidget(lb_MySqlPort);
    layMySqlPort->addWidget(le_MySqlPort);
    layMySqlPort->addStretch(1);
    QHBoxLayout *layMySqlName = new QHBoxLayout;
    layMySqlName->addWidget(lb_MySqlName);
    layMySqlName->addWidget(le_MySqlName);
    layMySqlName->addStretch(1);
    QHBoxLayout *layMySqlUser = new QHBoxLayout;
    layMySqlUser->addWidget(lb_MySqlUser);
    layMySqlUser->addWidget(le_MySqlUser);
    layMySqlUser->addStretch(1);
    QHBoxLayout *layMySqlPasswd = new QHBoxLayout;
    layMySqlPasswd->addWidget(lb_MySqlPasswd);
    layMySqlPasswd->addWidget(le_MySqlPasswd);
    layMySqlPasswd->addStretch(1);

    layMySql->addLayout(layMySqlHost);
    layMySql->addLayout(layMySqlPort);
    layMySql->addLayout(layMySqlName);
    layMySql->addLayout(layMySqlUser);
    layMySql->addLayout(layMySqlPasswd);

    le_MySqlPasswd->setEchoMode(QLineEdit::Password);

    QGroupBox *groupHttp = new QGroupBox("图片HTTP服务器设置");
    QVBoxLayout *layHttp = new QVBoxLayout(groupHttp);
    QLabel *lb_HttpHost = new QLabel("HTTP服务端地址：");
    le_HttpHost = new QLineEdit;

    QHBoxLayout *layHttpHost = new QHBoxLayout;
    layHttpHost->addWidget(lb_HttpHost);
    layHttpHost->addWidget(le_HttpHost);
    layHttpHost->addStretch(1);

    layHttp->addLayout(layHttpHost);


    QGroupBox *groupTcp = new QGroupBox("TCP设置");
    QVBoxLayout *layTcp = new QVBoxLayout(groupTcp);
    QLabel *lb_TcpHost = new QLabel("TCP服务端IP：");
    le_TcpHost = new QLineEdit;
    QLabel *lb_TcpPort = new QLabel("TCP服务端端口：");
    le_TcpPort = new QLineEdit;

    QHBoxLayout *layTcpHost = new QHBoxLayout;
    layTcpHost->addWidget(lb_TcpHost);
    layTcpHost->addWidget(le_TcpHost);
    layTcpHost->addStretch(1);
    QHBoxLayout *layTcpPort = new QHBoxLayout;
    layTcpPort->addWidget(lb_TcpPort);
    layTcpPort->addWidget(le_TcpPort);
    layTcpPort->addStretch(8);

    layTcp->addLayout(layTcpHost);
    layTcp->addLayout(layTcpPort);


    QGroupBox *groupTable = new QGroupBox("桌号设置：");
    QHBoxLayout *layTable = new QHBoxLayout(groupTable);
    QLabel *lb_TableNum = new QLabel("桌号：");
    le_TableNum = new QLineEdit;
    layTable->addWidget(lb_TableNum);
    layTable->addWidget(le_TableNum);
    layTable->addStretch(1);

    QHBoxLayout *layConfigBtns = new QHBoxLayout;
    QPushButton *btnRevConfig = new QPushButton("恢复设置");
    QPushButton *btnUpdateConfig = new QPushButton("更新设置");
    layConfigBtns->addStretch(2);
    layConfigBtns->addWidget(btnRevConfig);
    layConfigBtns->addStretch(4);
    layConfigBtns->addWidget(btnUpdateConfig);
    layConfigBtns->addStretch(2);


    layConfig->addWidget(groupMySql);
    layConfig->addWidget(groupHttp);
    layConfig->addWidget(groupTcp);
    layConfig->addWidget(groupTable);
    layConfig->addLayout(layConfigBtns);



    //初始化配置信息
    le_MySqlHost->setText(_dbHost);
    le_MySqlName->setText(_dbName);
    le_MySqlUser->setText(_dbUser);
    le_MySqlPort->setText(QString::number(_dbPort));
    le_MySqlPasswd->setText(_dbPasswd);

    le_HttpHost->setText(_picHost);

    le_TcpHost->setText(_tcpHost);
    le_TcpPort->setText(QString::number(_tcpPort));

    le_TableNum->setText(_tableNum);


    //绑定两按钮
    connect(btnUpdateConfig,&QPushButton::clicked,this,&DialogSettings::slotUpdateBtnClicked);
    connect(btnRevConfig,&QPushButton::clicked,this,&DialogSettings::slotRevBtnClicked);

}

void DialogSettings::slotUpdateBtnClicked()
{
    if(le_MySqlHost->text().isEmpty()||le_MySqlName->text().isEmpty()||le_MySqlUser->text().isEmpty()||le_MySqlPasswd->text().isEmpty()||le_TcpHost->text().isEmpty()||le_TcpPort->text().isEmpty())
    {
        QMessageBox::critical(this,"错误","关键信息不完整！");
        return;
    }

    //更新信息
    _dbHost = le_MySqlHost->text();
    _dbName = le_MySqlName->text();
    _dbUser = le_MySqlUser->text();
    _dbPasswd = le_MySqlPasswd->text();
    _dbPort = le_MySqlPort->text().toInt();
    _picHost = le_HttpHost->text();
    _tcpHost = le_TcpHost->text();
    _tcpPort = le_TcpPort->text().toInt();
    _tableNum = le_TableNum->text().toInt();

    WriteJson jsonConfig(_dbHost,_dbName,_dbUser,_dbPasswd,_dbPort,_picHost,_tcpHost,_tcpPort,_tableNum);
    if(!jsonConfig.writeToFile())
    {
        QMessageBox::critical(this,"错误","无法更新配置！");
    }

    /* 申请一个临时数据库，检测数据库配置信息和保存tcp信息 */
    QSqlDatabase tmpdb = QSqlDatabase::addDatabase("QMYSQL");
    tmpdb.setHostName(_dbHost);
    tmpdb.setPort(_dbPort);
    tmpdb.setUserName(_dbUser);
    tmpdb.setDatabaseName(_dbName);
    tmpdb.setPassword(_dbPasswd);

    if(tmpdb.open())
    {
        QMessageBox::information(this,"客户端配置信息更新成功！","客户端配置信息更新成功，请重启客户端程序！");

        exit(0);
    }
    else
    {
        QMessageBox::critical(this,"无法连接MySql数据库","无法连接MySql数据库，请检查相关信息！");
        qDebug()<<tmpdb.lastError().text();
    }
}

void DialogSettings::slotRevBtnClicked()
{
    le_MySqlHost->setText(_dbHost);
    le_MySqlPort->setText(QString::number(_dbPort));
    le_MySqlName->setText(_dbName);
    le_MySqlUser->setText(_dbUser);
    le_MySqlPasswd->setText(_dbPasswd);

    le_TcpHost->setText(_tcpHost);
    le_TcpPort->setText(QString::number(_tcpPort));
}
