#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMainWindow>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QPushButton>
#include <QSound>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlIndex>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QTabWidget>
#include <QTableView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>

#include "Dish.h"
#include "Operation.h"
#include "SimpleWebServer.h"
#include "TcpServer.h"
#include "dialogaddrecord.h"
#include "dialogeditrecord.h"
#include "dialoghistoryviewer.h"
#include "dialogordersviewer.h"
#include "httpfileupdate.h"
#include "json.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
	class ServerMainWindow;
}
QT_END_NAMESPACE

class ServerMainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit ServerMainWindow(QWidget *parent = nullptr);
	~ServerMainWindow();

private:
	// Sound
	QSound *sound;

private:
	// Setting
	void loadSetting();

	// UI
	void initUI();

	// Menu
	void generateUpdatedMenu();

private:
	// StatusBar
	QLabel *labelOrdersCount;
	QLabel *labelOrdersNoCount;
	QLabel *labelTime;
	QLabel *labelClientCount;
	QTimer *timer;

private:
	// Orders Tab
	QTableWidget *_table_Orders;

private:
	// Menu Tab
	QTableView *_view_Menu;
	QSqlTableModel *_model;
	QSqlTableModel *_menuTypeModel;

	QList<QString> _menuTypeList;// 菜品种类列表
	QString oldDishType;
	QVector<Dish> _dishes;                         // 菜品列表
	QHash<QString, int> _menuTypeNumHash;          // 菜品种类与数量键值对
	QVector<DianaJustEat::Operation *> _operations;// 操作列表

private:
	// Config Tab
	QLineEdit *le_MySqlHost;
	QLineEdit *le_MySqlName;
	QLineEdit *le_MySqlUser;
	QLineEdit *le_MySqlPasswd;
	QLineEdit *le_MySqlPort;

	QLineEdit *le_HttpHost;

	QLineEdit *le_TcpHost;
	QLineEdit *le_TcpPort;
	QLineEdit *le_TcpStatusPort;

	QLineEdit *le_ClearShot;

private:
	// DataBase
	bool connectDb();//connect  to database

	// DataBase Info
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

private:
	// TCP
	bool startTcpServer();
	TcpServer *tcpServer;

	// TCP Info
	QString _tcpHost;
	int _tcpPort;
	int _tcpStatusPort;

private:
	// Web服务器
	void startWebServer();

	SimpleWebServer *_webServer;
	QThread *_webThread;

public slots:
	void slotBtnViewerClicked();
	void slotBtnHistoryClicked();
	void slotBtnHandleClicked();
	void slotBtnReHandleClicked();

	void slotBtnEditClicked();
	void slotBtnAddClicked();
	void slotBtnDelClicked();

	void slotSubmit(QString dishName, QString dishType, QString dishInfo, QString dishPrice, QString dishPhoto);
	void slotUpdate(int dishId, const QString dishName, const QString dishType, const QString dishInfo, const double dishPrice, const QString dishPhoto, bool photoUpdated);


	void slotUpdateBtnClicked();
	void slotRevBtnClicked();


	void closeEvent(QCloseEvent *);//重写退出事件

public slots:
	// 菜单相关信号槽
	void slotQueryMenu(QTcpSocket *target);   // 客户端请求菜单
	void slotNewOrder(const QByteArray &menu);// 客户端发送新订单

signals:
	void sigMenuUpdate(const QByteArray &menu);//更新客户端菜单信号

private:
	Ui::ServerMainWindow *ui;

private:
	//图片
	QDir _picPath;

private:
	int _OrdersCount = 0;
	int _OrdersNoCount = 0;

	int _clearShot;
};
#endif// SERVERMAINWINDOW_H
