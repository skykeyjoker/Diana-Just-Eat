#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QDateTime>
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
#include <QStatusBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QThreadPool>
#include <QTimer>
#include <QToolBox>
#include <QVBoxLayout>

#include "Dish.h"
#include "cartitem.h"
#include "dialogcartview.h"
#include "dialogcheckout.h"
#include "dialogsettings.h"
#include "httpfiledownload.h"
#include "json.hpp"
#include "mybutton.h"
#include "tcpclient.h"


QT_BEGIN_NAMESPACE
namespace Ui {
	class ClientMainWindow;
}
QT_END_NAMESPACE

class ClientMainWindow : public QMainWindow {
	Q_OBJECT

private:
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
	QList<CartItem> _cartLists;
	QSet<QString> _cartSet;
	int _cartNumCount = 0;
	double _cartPriceCount = 0;

private:
	// 初始化界面
	void initUI();

	Ui::ClientMainWindow *ui;

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

	void slotReadyCheckOut(const QString &note);

	void slotDisconnectedToServer();


private:
	//图片服务器
	QString _picHost;

	//Menu
	int _menuCount;
	int _alreadyDownloadMenuCount;

	QDir _picPath;
	QString _tableNum;
};
#endif// CLIENTMAINWINDOW_H
