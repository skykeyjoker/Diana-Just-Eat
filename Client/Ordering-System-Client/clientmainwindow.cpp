#include "clientmainwindow.h"
#include "./ui_clientmainwindow.h"

ClientMainWindow::ClientMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientMainWindow)
{
    ui->setupUi(this);

    TcpClient *client = new TcpClient;

    client->establishConnect("127.0.0.1",8081);




    connect(ui->pushButton,&QPushButton::clicked,[=](){

        QStringList strlist;
        strlist<<ui->lineTable->text()<<";"<<ui->linePrice->text()<<";"<<ui->lineMenu->text()<<";"<<ui->lineNote->text();

        QString str;
        foreach(QString s,strlist)
        {
            str+=s;
        }
        qDebug()<<str;
        client->sendData(str.toUtf8());
    });

}

ClientMainWindow::~ClientMainWindow()
{
    delete ui;
}

