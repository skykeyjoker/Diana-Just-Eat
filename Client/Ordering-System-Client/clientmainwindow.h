#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBox>
#include <QGroupBox>
#include <QTextBrowser>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QLineEdit>
#include <QPixmap>
#include <QMessageBox>
#include <QDebug>


#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>


#include "tcpclient.h"
#include "readjson.h"
#include "dialogsettings.h"
#include "httpfiledownload.h"


QT_BEGIN_NAMESPACE
namespace Ui { class ClientMainWindow; }
QT_END_NAMESPACE

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

    bool connectDb();

    //菜单显示
    QListWidget *_menuList = new QListWidget;

    QStringList _menuTypeList;
    QList<QString> _menuNameList;
    QList<int> _menuTypeNumList;
    QList<QString> _menuFileNameList;

    QDir _picPath;

    void loadMenu();



    void insertItems();


    //菜品详细显示
    QGroupBox *_viewGroupBox;
    QLabel *lb_pic;
    QLabel *lb_dishName;
    QLabel *lb_dishNameContent;
    QLabel *lb_dishPrice;
    QLabel *lb_dishPriceContent;
    QLabel *lb_dishInfo;
    QTextBrowser *tb_dishInfo;

    TcpClient *client;

public:
    ClientMainWindow(QWidget *parent = nullptr);
    ~ClientMainWindow();

private slots:
    void on_actionSetting_triggered();
    void slotUpdateMenu();
    void slotItemClicked(QListWidgetItem *item);
    void slotAddAlreadyDownloadMenuCount();

signals:
    void signalAddAlreadyDownloadMenuCount();
private:
    Ui::ClientMainWindow *ui;

    //Database
    QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL");
    QString _dbHost;
    QString _dbName;
    QString _dbUser;
    QString _dbPasswd;
    int _dbPort;

    //Tcp
    QString _tcpHost;
    int _tcpPort;

    //图片服务器
    QString _picHost;

    //Menu
    int _menuCount;
    int _alreadyDownloadMenuCount;
};
#endif // CLIENTMAINWINDOW_H
