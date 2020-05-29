#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QCloseEvent>
#include <QGroupBox>


#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTableView>
#include <QHeaderView>

#include "readjson.h"
#include "writejson.h"
#include "dialogaddrecord.h"
#include "dialogeditrecord.h"


QT_BEGIN_NAMESPACE
namespace Ui
{
    class ServerMainWindow;
}
QT_END_NAMESPACE

class ServerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerMainWindow(QWidget *parent = nullptr);
    ~ServerMainWindow();

    // DataBase
    ReadJson dbInfo;
    bool connectDb(); //connect  to database

    QListWidget *_list_Orders;


    // Menu Tab
    QTableView *_view_Menu;
    QSqlTableModel *_model;


    // Config Tab
    QLineEdit *le_MySqlHost;
    QLineEdit *le_MySqlName;
    QLineEdit *le_MySqlUser;
    QLineEdit *le_MySqlPasswd;
    QLineEdit *le_MySqlPort;

    QLineEdit *le_TcpHost;
    QLineEdit *le_TcpPort;


public slots:
    void slotBtnEditClicked();
    void slotBtnAddClicked();
    void slotBtnDelClicked();

    void slotSubmit(QString dishName, QString dishType, QString dishInfo, QString dishPrice, QByteArray dishPhoto);
    void slotUpdate(int dishId, QString dishName, QString dishType, QString dishInfo, QString dishPrice, QByteArray dishPhoto);


    void slotUpdateBtnClicked();
    void slotRevBtnClicked();


    void closeEvent(QCloseEvent *); //重写退出事件
private:
    Ui::ServerMainWindow *ui;

    // DataBase Info
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    QString _dbHost;
    QString _dbName;
    QString _dbUser;
    QString _dbPasswd;
    int _dbPort;
    QString _tcpHost;
    int _tcpPort;

};
#endif // SERVERMAINWINDOW_H
