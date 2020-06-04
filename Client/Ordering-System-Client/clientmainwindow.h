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
#include <QTimer>


#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>


#include "tcpclient.h"
#include "readjson.h"
#include "dialogsettings.h"
#include "dialogcartview.h"
#include "dialogcheckout.h"
#include "httpfiledownload.h"
#include "cartitem.h"
#include "timestamp.h"
#include "mybutton.h"


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

    MyButton *btn_cart;


    //TCP
    TcpClient *client;


    //状态栏
    QLabel *lb_cartNumCount;
    QLabel *lb_cartPriceCount;
    QLabel *lb_currentTime;
    QTimer *timer;

public:
    ClientMainWindow(QWidget *parent = nullptr);
    ~ClientMainWindow();

public slots:
    void on_actionSetting_triggered();
    void slotUpdateMenu();
    void slotItemClicked(QListWidgetItem *item);
    void slotAddAlreadyDownloadMenuCount();

    void slotAddtoCart();

    void slotCartBtnClicked();

    void slotCartChanged(QList<CartItem>cartlist);
    void slotCartCleaned();

    void slotCartCheckOut();

    void slotReadyCheckOut(QString note);

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

    QDir _picPath;
    QString _tableNum;

    QList<CartItem> cartLists;
    int _cartNumCount=0;
    double _cartPriceCount=0;
};
#endif // CLIENTMAINWINDOW_H
