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

#include "SimpleWebServer.h"
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

private:
	// StatusBar
	QLabel *labelOrdersCount;
	QLabel *labelOrdersNoCount;
	QLabel *labelTime;
	QTimer *timer;

private:
	// Orders Tab
	QTableWidget *_table_Orders;

private:
	// Menu Tab
	QTableView *_view_Menu;
	QSqlTableModel *_model;
	QSqlTableModel *_menuTypeModel;
	QList<QString> _menuTypeList;
	QString oldDishType;

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

	// TCP Info
	QString _tcpHost;
	int _tcpPort;
	int _tcpStatusPort;

	// 订单菜单信道
	QTcpServer *_tcpServer;
	QList<QTcpSocket *> _tcpClients;

	// 状态信道
	QTcpServer *_tcpStatusServer;
	QList<QTcpSocket *> _tcpStatusClients;

private:
	// Web服务器
	void startWebServer();

	SimpleWebServer *_webServer;
	QThread *_webThread;

public slots:
	void slotNewConnection();
	void slotReadyRead();
	void slotStatusNewConnection();
	void slotStatusReadyRead();

	void slotSendMenuUpdateMessage();

	void slotBtnViewerClicked();
	void slotBtnHistoryClicked();
	void slotBtnHandleClicked();
	void slotBtnReHandleClicked();

	void slotBtnEditClicked();
	void slotBtnAddClicked();
	void slotBtnDelClicked();

	void slotSubmit(QString dishName, QString dishType, QString dishInfo, QString dishPrice, QString dishPhoto);
	void slotUpdate(int dishId, QString dishName, QString dishType, QString dishInfo, QString dishPrice, QString dishPhoto);


	void slotUpdateBtnClicked();
	void slotRevBtnClicked();


	void closeEvent(QCloseEvent *);//重写退出事件

signals:
	void sendMenuUpdateSignal();//更新客户端菜单信号

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
