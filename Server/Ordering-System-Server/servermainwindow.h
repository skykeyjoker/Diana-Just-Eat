#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include<QDebug>


#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTableView>
#include <QHeaderView>


#include "readjson.h"
#include "dialogaddrecord.h"
#include "dialogeditrecord.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ServerMainWindow; }
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


    QTableView *_view_Menu;
    QSqlTableModel *_model;

public slots:
    void slotBtnEditClicked();
    void slotBtnAddClicked();
    void slotBtnDelClicked();

private:
    Ui::ServerMainWindow *ui;

    // DataBase Info
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    QString _dbHost;
    QString _dbName;
    QString _dbUser;
    QString _dbPasswd;
    int _dbPort;
};
#endif // SERVERMAINWINDOW_H
