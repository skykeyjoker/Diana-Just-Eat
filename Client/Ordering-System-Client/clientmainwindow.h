#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QThreadPool>
#include <QTimer>
#include <QToolBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "Dish.h"
#include "cartitem.h"
#include "dialogcartview.h"
#include "dialogcheckout.h"
#include "dialogsettings.h"
#include "httpfiledownload.h"
#include "json.hpp"
#include "mybutton.h"
#include "readjson.h"
#include "tcpclient.h"
#include "timestamp.h"


// TODO 删除UI文件
QT_BEGIN_NAMESPACE
namespace Ui {
	class ClientMainWindow;
}
QT_END_NAMESPACE

class ClientMainWindow : public QMainWindow {
	Q_OBJECT

private:
	// TODO 架构改变，客户端无需接触到数据库操作；维护一个菜单列表即可（内存占用可能较大，但效率较高）
	void loadSetting();// 读取配置文件

private:
	//菜单显示
	void loadMenu();

	void showMenu();

	QListWidget *_menuList;

	QList<Dish> _dishes;

	QStringList _menuTypeList;
	QHash<QString, int> _menuTypeNumHash;
	QHash<QString, QString> _dishNameAndFileNameHash;

private:
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

private:
	//TCP
	TcpClient *client;

	QString _tcpHost;
	int _tcpPort;
	int _tcpStatusPort;// 存活状态信道端口

private:
	//状态栏
	QLabel *lb_cartNumCount;
	QLabel *lb_cartPriceCount;
	QLabel *lb_currentTime;
	QTimer *timer;

private:
	// 购物车
	QList<CartItem> cartLists;
	int _cartNumCount = 0;
	double _cartPriceCount = 0;

private:
	// 初始化界面
	void initUI();

public:
	explicit ClientMainWindow(QWidget *parent = nullptr);
	~ClientMainWindow();

public slots:
	void on_actionSetting_triggered();
	void slotUpdateMenu(const QByteArray data);
	void slotQueryMenu(const QByteArray data);
	void slotItemClicked(QListWidgetItem *item);

	void slotAddtoCart();

	void slotCartBtnClicked();

	void slotCartChanged(QList<CartItem> cartlist);
	void slotCartCleaned();

	void slotCartCheckOut();

	void slotReadyCheckOut(QString note);

	void slotDisconnectedToServer();


private:
	Ui::ClientMainWindow *ui;

	//图片服务器
	QString _picHost;

	//Menu
	int _menuCount;
	int _alreadyDownloadMenuCount;

	QDir _picPath;
	QString _tableNum;
};
#endif// CLIENTMAINWINDOW_H
