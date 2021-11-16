#include "servermainwindow.h"

#include "./ui_servermainwindow.h"


ServerMainWindow::ServerMainWindow(QWidget *parent)
	: QMainWindow(parent),
	  ui(new Ui::ServerMainWindow) {
	ui->setupUi(this);

	setWindowTitle("Diana Just Eat");

	// 读取配置文件并连接数据库
	loadSetting();// 读取配置文件

	bool dbRet = connectDb();// 连接数据库
	if (!dbRet) {
		QMessageBox::critical(this, "服务端启动失败", "无法连接数据库，请编辑目录下的config.json文件！");
		exit(1);
	}


	// 创建Tcp服务器
	bool tcpRet = startTcpServer();
	if (!tcpRet) {
		QMessageBox::critical(this, "Tcp服务端未建立", "未能建立Tcp服务端!");
		exit(1);
	} else
		qDebug() << "成功启动TCP服务端";

	// 获取图片目录
	if (!QDir::current().exists("Pic"))
		QDir::current().mkdir("Pic");
	_picPath = QDir(QDir::currentPath() + "/Pic");

	// 启动web服务器
	startWebServer();

	//收到sendMenuUpdateSiganl后调用slotSendMenuUpdateMessage槽函数
	//connect(this, &ServerMainWindow::sendMenuUpdateSignal, this, &ServerMainWindow::slotSendMenuUpdateMessage);

	// 分离初始化UI
	initUI();
}

ServerMainWindow::~ServerMainWindow() {
	delete ui;
}

void ServerMainWindow::loadSetting() {
	QFile settingFile("config.json");
	QByteArray settingFileData;

	if (settingFile.open(QFile::ReadOnly)) {
		settingFileData = settingFile.readAll();

		using Json = nlohmann::json;
		Json settingFileJson;
		settingFileJson = Json::parse(settingFileData.data(), nullptr, false);

		// 赋值TCP服务端消息
		_tcpHost = QString::fromStdString(settingFileJson["tcpHost"].get<std::string>());
		_tcpPort = settingFileJson["tcpPort"].get<int>();
		_tcpStatusPort = settingFileJson["tcpStatusPort"].get<int>();
		// 赋值清除时间
		_clearShot = settingFileJson["clearShot"].get<int>();
	} else {
		qDebug() << "打开配置文件失败";
		QMessageBox::critical(this, "启动失败", "打开配置文件失败");
	}
}

bool ServerMainWindow::connectDb() {
	db.setDatabaseName("MenuInfo.db");

	bool dbRet = db.open();

	if (!dbRet) {
		qDebug() << "数据库连接失败" << db.lastError().text();
		return false;
	} else
		qDebug() << "数据库连接成功";

	// 设置QSqlModel
	// menu表
	_model = new QSqlTableModel;
	_model->setTable("Menu");
	_model->select();
	_model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置提交策略

	// menuType表
	_menuTypeModel = new QSqlTableModel;
	_menuTypeModel->setTable("MenuType");
	_menuTypeModel->select();
	_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

	// 遍历menuType表初始化_menuTypeList
	for (int i = 0; i < _menuTypeModel->rowCount(); i++) {
		QSqlRecord currentRecord = _menuTypeModel->record(i);
		QString currentMenuTypeName = currentRecord.value(1).toString();
		_menuTypeList.append(currentMenuTypeName);
	}

	qDebug() << "_menuTypeList: " << _menuTypeList;

	return true;
}

bool ServerMainWindow::startTcpServer() {
	bool ret;
	tcpServer = new TcpServer(_tcpHost, _tcpPort, _tcpStatusPort);

	ret = tcpServer->establishServer();

	// 菜单请求
	connect(tcpServer, qOverload<QTcpSocket *>(&TcpServer::sigQueryMenu),
			this, qOverload<QTcpSocket *>(&ServerMainWindow::slotQueryMenu));

	// 新订单
	connect(tcpServer, qOverload<const QByteArray &>(&TcpServer::sigNewOrder),
			this, qOverload<const QByteArray &>(&ServerMainWindow::slotNewOrder));

	return ret;
}

void ServerMainWindow::startWebServer() {
	_webServer = new SimpleWebServer(_picPath.path());
	_webThread = new QThread;

	_webServer->moveToThread(_webThread);
	_webThread->start();
}

void ServerMainWindow::initUI() {
	ui->tabWidget->setCurrentIndex(0);

	// Init Tab of Orders
	QVBoxLayout *layOrders = new QVBoxLayout(ui->tab_Orders);
	_table_Orders = new QTableWidget;
	layOrders->addWidget(_table_Orders);
	//订单号 桌号 价格 订单内容 订单备注
	_table_Orders->setColumnCount(6);//设置列数
	_table_Orders->setHorizontalHeaderLabels(QStringList() << "状态"
														   << "订单号"
														   << "桌号"
														   << "价格"
														   << "订单内容"
														   << "订单备注");//设置水平表头

	//设置列宽
	_table_Orders->setColumnWidth(0, 120);
	_table_Orders->setColumnWidth(1, 200);
	_table_Orders->setColumnWidth(2, 90);
	_table_Orders->setColumnWidth(3, 90);
	_table_Orders->setColumnWidth(4, 300);
	_table_Orders->setColumnWidth(5, 280);

	//让“订单内容”和订单备注随拉伸变化
	_table_Orders->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
	_table_Orders->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);

	//设置table属性
	_table_Orders->setFocusPolicy(Qt::NoFocus);                         //失去焦点，让_table_Orders在未选中前不能删除
	_table_Orders->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行行为，以行为单位
	_table_Orders->setSelectionMode(QAbstractItemView::SingleSelection);//设置选择模式，选择单行
	_table_Orders->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置为禁止编辑

	QHBoxLayout *layOrder_Btns = new QHBoxLayout;
	QPushButton *btnViewer = new QPushButton("查看该订单");
	QPushButton *btnReHandle = new QPushButton("重新处理该订单");
	QPushButton *btnHandle = new QPushButton("处理该订单");
	QPushButton *btnHistory = new QPushButton("查看历史订单");
	layOrder_Btns->addStretch(2);
	layOrder_Btns->addWidget(btnHistory);
	layOrder_Btns->addStretch(4);
	layOrder_Btns->addWidget(btnViewer);
	layOrder_Btns->addStretch(4);
	layOrder_Btns->addWidget(btnHandle);
	layOrder_Btns->addStretch(4);
	layOrder_Btns->addWidget(btnReHandle);
	layOrder_Btns->addStretch(2);
	layOrders->addLayout(layOrder_Btns);

	//绑定双击事件
	//connect(_table_Orders,SIGNAL(itemDoubleClicked(QTableWidgetItem *item)),this,SLOT(slotItemDoubleClicked(QTableWidgetItem *item)));
	//connect(_table_Orders,&QTableWidget::doubleClicked(const QModelIndex &index));
	//connect(_table_Orders,SIGNAL(SIGNAL(cellDoubleClicked(int , int ))),this,SLOT(slotCellDoubleClicked(int , int )));
	//TODO 绑定双击事件

	//绑定按钮
	connect(btnHistory, &QPushButton::clicked, this, &ServerMainWindow::slotBtnHistoryClicked);
	connect(btnReHandle, &QPushButton::clicked, this, &ServerMainWindow::slotBtnReHandleClicked);
	connect(btnHandle, &QPushButton::clicked, this, &ServerMainWindow::slotBtnHandleClicked);
	connect(btnViewer, &QPushButton::clicked, this, &ServerMainWindow::slotBtnViewerClicked);


	// Init Tab of Menu
	// TODO 添加菜品种类编辑
	QVBoxLayout *layMenu = new QVBoxLayout(ui->tab_Menu);
	_view_Menu = new QTableView;
	layMenu->addWidget(_view_Menu);

	//设置_view_Menu表头
	_model->setHeaderData(0, Qt::Horizontal, "菜品编号");
	_model->setHeaderData(1, Qt::Horizontal, "菜品名称");
	_model->setHeaderData(2, Qt::Horizontal, "菜品种类");
	_model->setHeaderData(3, Qt::Horizontal, "菜品描述");
	_model->setHeaderData(4, Qt::Horizontal, "菜品价格");
	_model->setHeaderData(5, Qt::Horizontal, "菜品图片");
	_view_Menu->setModel(_model);//绑定model

	//设置_view_Menu属性
	_view_Menu->setColumnHidden(0, true);                                         //隐藏ID列
	_view_Menu->setAlternatingRowColors(true);                                    //行颜色变换
	_view_Menu->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);//让“描述”随拉伸变化
	_view_Menu->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);//让“图片”随拉伸变化
	_view_Menu->setColumnWidth(3, 250);
	_view_Menu->setColumnWidth(5, 250);
	_view_Menu->setSelectionBehavior(QAbstractItemView::SelectRows); //设置为选择单位为行
	_view_Menu->setSelectionMode(QAbstractItemView::SingleSelection);//设置选择一行
	_view_Menu->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置为只读

	QHBoxLayout *layMenu_btn = new QHBoxLayout(NULL);
	QPushButton *btnEdit = new QPushButton("修改菜品");
	QPushButton *btnAdd = new QPushButton("添加菜品");
	QPushButton *btnDel = new QPushButton("删除菜品");
	// TODO 添加菜品种类编辑按钮
	QPushButton *btnTypeEdit = new QPushButton("编辑菜品种类");

	layMenu_btn->addWidget(btnEdit);
	layMenu_btn->addSpacing(15);
	layMenu_btn->addWidget(btnAdd);
	layMenu_btn->addSpacing(15);
	layMenu_btn->addWidget(btnDel);
	layMenu_btn->addSpacing(15);
	layMenu_btn->addWidget(btnTypeEdit);
	layMenu->addLayout(layMenu_btn);


	//绑定按钮信号与槽
	// TODO 添加菜品种类编辑按钮
	connect(btnEdit, &QPushButton::clicked, this, &ServerMainWindow::slotBtnEditClicked);
	connect(btnAdd, &QPushButton::clicked, this, &ServerMainWindow::slotBtnAddClicked);
	connect(btnDel, &QPushButton::clicked, this, &ServerMainWindow::slotBtnDelClicked);


	// Init Tab for Config
	QVBoxLayout *layConfig = new QVBoxLayout(ui->tab_Settings);

	// TCP设置区
	QGroupBox *groupTcp = new QGroupBox("TCP设置");
	QVBoxLayout *layTcp = new QVBoxLayout(groupTcp);
	QLabel *lb_TcpHost = new QLabel("TCP服务端IP：");
	le_TcpHost = new QLineEdit;
	QLabel *lb_TcpPort = new QLabel("TCP服务端端口：");
	le_TcpPort = new QLineEdit;
	QLabel *lb_TcpStatusPort = new QLabel("TCP服务端状态信道端口：");
	le_TcpStatusPort = new QLineEdit;

	QHBoxLayout *layTcpHost = new QHBoxLayout;
	layTcpHost->addWidget(lb_TcpHost);
	layTcpHost->addWidget(le_TcpHost);
	layTcpHost->addStretch(1);
	QHBoxLayout *layTcpPort = new QHBoxLayout;
	layTcpPort->addWidget(lb_TcpPort);
	layTcpPort->addWidget(le_TcpPort);
	layTcpPort->addStretch(8);
	QHBoxLayout *layTcpStatusPort = new QHBoxLayout;
	layTcpStatusPort->addWidget(lb_TcpStatusPort);
	layTcpStatusPort->addWidget(le_TcpStatusPort);
	layTcpStatusPort->addStretch(8);

	layTcp->addLayout(layTcpHost);
	layTcp->addLayout(layTcpPort);
	layTcp->addLayout(layTcpStatusPort);

	// 订单系统设置区
	QGroupBox *groupOrders = new QGroupBox("订单系统设置");
	QVBoxLayout *layConfigOrders = new QVBoxLayout(groupOrders);

	QHBoxLayout *layConfigOrdersClearShot = new QHBoxLayout;
	QLabel *lb_CLearShot = new QLabel("已处理订单消失等待时间(s)：");
	le_ClearShot = new QLineEdit;
	layConfigOrdersClearShot->addWidget(lb_CLearShot);
	layConfigOrdersClearShot->addWidget(le_ClearShot);
	layConfigOrdersClearShot->addStretch(1);

	layConfigOrders->addLayout(layConfigOrdersClearShot);

	// 按钮区
	QHBoxLayout *layConfigBtns = new QHBoxLayout;
	QPushButton *btnRevConfig = new QPushButton("恢复设置");
	QPushButton *btnUpdateConfig = new QPushButton("更新设置");
	layConfigBtns->addStretch(2);
	layConfigBtns->addWidget(btnRevConfig);
	layConfigBtns->addStretch(4);
	layConfigBtns->addWidget(btnUpdateConfig);
	layConfigBtns->addStretch(2);


	layConfig->addWidget(groupTcp);
	layConfig->addWidget(groupOrders);
	layConfig->addLayout(layConfigBtns);


	//初始化配置信息
	le_TcpHost->setText(_tcpHost);
	le_TcpPort->setText(QString::number(_tcpPort));
	le_TcpStatusPort->setText(QString::number(_tcpStatusPort));

	le_ClearShot->setText(QString::number(_clearShot));

	//绑定两按钮
	connect(btnUpdateConfig, &QPushButton::clicked, this, &ServerMainWindow::slotUpdateBtnClicked);
	connect(btnRevConfig, &QPushButton::clicked, this, &ServerMainWindow::slotRevBtnClicked);


	// Status Bar
	labelOrdersNoCount = new QLabel(tr("未处理订单：%1").arg(QString::number(_OrdersNoCount)), this);
	labelOrdersCount = new QLabel(tr("总订单：%1").arg(QString::number(_OrdersCount)), this);
	labelTime = new QLabel(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), this);
	labelClientCount = new QLabel(tr("在线客户端：%1").arg(QString::number(tcpServer->getCurrentClientNum())), this);

	ui->statusbar->addPermanentWidget(labelOrdersNoCount);
	ui->statusbar->addPermanentWidget(labelOrdersCount);
	ui->statusbar->addPermanentWidget(labelClientCount);
	ui->statusbar->addPermanentWidget(labelTime);

	//启动计时器
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, [=]() {
		labelTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
		labelClientCount->setText(tr("在线客户端：%1").arg(QString::number(tcpServer->getCurrentClientNum())));
	});
	timer->start(1000);

	//Sound
	sound = new QSound(":/Res/clock.wav");
}

void ServerMainWindow::slotQueryMenu(QTcpSocket *target) {
	// 客户端请求菜单处理
	QByteArray sendData;
	using Json = nlohmann::json;
	Json sendDataJson;
	sendDataJson["code"] = 0;

	// 插入menu数组
	Json menuArr = Json::array();
	if (_dishes.isEmpty()) {
		// 首次发送菜单，构造菜单列表
		for (int i = 0; i < _model->rowCount(); ++i) {
			auto currentDishRecord = _model->record(i);
			QString currentDishName = currentDishRecord.value("Name").toString();
			QString currentDishType = currentDishRecord.value("Type").toString();
			QString currentDishInfo = currentDishRecord.value("Info").toString();
			double currentDishPrice = currentDishRecord.value("Price").toDouble();
			QString currentDishPhoto = currentDishRecord.value("Photo").toString();

			Dish currentDish(currentDishName, currentDishType, currentDishInfo, currentDishPrice, currentDishPhoto);
			_dishes.push_back(currentDish);
		}
	}
	// 已有菜单列表
	for (auto currentDish : _dishes) {
		Json currentDishJson;
		currentDishJson["Name"] = currentDish.getName().toStdString();
		currentDishJson["Type"] = currentDish.getType().toStdString();
		currentDishJson["Info"] = currentDish.getInfo().toStdString();
		currentDishJson["Price"] = currentDish.getPrice();
		currentDishJson["Photo"] = currentDish.getPhoto().toStdString();

		menuArr.push_back(currentDishJson);
	}
	sendDataJson["menu"] = menuArr;

	// 插入menuType数组
	Json menuTypeArr = Json::array();
	if (_menuTypeNumHash.isEmpty()) {
		// 首次发送菜单，构造菜单种类数量对应表
		for (int i = 0; i < _menuTypeModel->rowCount(); ++i) {
			auto currentMenuTypeRecord = _menuTypeModel->record(i);
			QString currentMenuTypeName = currentMenuTypeRecord.value("TypeName").toString();
			int currentMenuTypeNum = currentMenuTypeRecord.value("Num").toInt();

			_menuTypeNumHash[currentMenuTypeName] = currentMenuTypeNum;
		}
	}
	// 已有菜单种类数量对应表
	for (auto currentMenuType : _menuTypeNumHash.keys()) {
		Json currentMenuTypeJson;
		currentMenuTypeJson["Name"] = currentMenuType.toStdString();
		currentMenuTypeJson["Num"] = _menuTypeNumHash[currentMenuType];

		menuTypeArr.push_back(currentMenuTypeJson);
	}
	sendDataJson["menuType"] = menuTypeArr;

	// JSON构造完成，发送数据
	sendData = QString::fromStdString(sendDataJson.dump(2)).toUtf8();
	tcpServer->sendMenu(target, sendData);
}

// TODO 此处重写为订单到达槽
void ServerMainWindow::slotNewOrder(const QByteArray &menu) {
	qDebug() << "收到新订单" << menu;
	using Json = nlohmann::json;
	Json menuJson;
	menuJson = Json::parse(menu.data(), nullptr, false);

	// 先在订单的tableWidget中插入一行
	_table_Orders->setRowCount(_table_Orders->rowCount() + 1);

	// 赋值新插入的_table_Orders的新行
	_table_Orders->setItem(_table_Orders->rowCount() - 1, 0, new QTableWidgetItem("未处理"));//每行第0列是订单状态

	// 订单号
	QString strOrderNum = QString::fromStdString(menuJson["OrderNum"].get<std::string>());
	_table_Orders->setItem(_table_Orders->rowCount() - 1, 1, new QTableWidgetItem(strOrderNum));//每行第1列是订单号

	// 桌号
	QString table = QString::fromStdString(menuJson["Table"].get<std::string>());
	QTableWidgetItem *itemTable = new QTableWidgetItem(table);
	_table_Orders->setItem(_table_Orders->rowCount() - 1, 2, itemTable);//每行第2列是桌号

	// 订单总价格
	QString price = QString::number(menuJson["Price"].get<double>());
	QTableWidgetItem *itemPrice = new QTableWidgetItem(price);          //订单总价格
	_table_Orders->setItem(_table_Orders->rowCount() - 1, 3, itemPrice);//每行第3列订单总价格

	// 订单内容
	QString orderStr;
	Json cartArr = menuJson["Carts"];
	// 构造订单内容
	for (int i = 0; i < cartArr.size(); ++i) {
		QString currentCartName = QString::fromStdString(cartArr[i]["Name"].get<std::string>());
		QString currentCartNum = QString::number(cartArr[i]["Num"].get<int>());
		orderStr.push_back('[');
		orderStr.push_back(currentCartName);
		orderStr.push_back(':');
		orderStr.push_back(currentCartNum);
		orderStr.push_back("]");
		if (i != cartArr.size() - 1)
			orderStr.push_back(',');
	}
	QTableWidgetItem *itemOrders = new QTableWidgetItem(orderStr);       // 订单内容
	_table_Orders->setItem(_table_Orders->rowCount() - 1, 4, itemOrders);//每行第4列是订单内容

	// 订单备注
	QString note = QString::fromStdString(menuJson["Note"].get<std::string>());
	QTableWidgetItem *itemNote = new QTableWidgetItem(note);           //订单备注
	_table_Orders->setItem(_table_Orders->rowCount() - 1, 5, itemNote);//每行第5列是订单备注

	//更新订单统计信息
	_OrdersCount++;  //总订单数加1
	_OrdersNoCount++;//未处理订单数加1

	//更新状态栏
	ui->statusbar->showMessage("有新的订单！", 2000);
	labelOrdersNoCount->setText(tr("未处理订单数：%1").arg(QString::number(_OrdersNoCount)));
	labelOrdersCount->setText(tr("总订单数：%1").arg(QString::number(_OrdersCount)));


	//Sound播放订单到达提醒声音
	sound->play();
}

// TODO 此处重写为菜单更新消息发送
//void ServerMainWindow::slotSendMenuUpdateMessage() {
//	qDebug() << "slotSendMenuUpdateMessage";
//	// TODO 重构菜单更新消息发送
//
//	QTcpSocket *currentSocket;
//
//	for (int i = 0; i < _tcpClients.size(); i++)//遍历每一个客户端socket,发送信息
//	{
//		currentSocket = _tcpClients.at(i);
//		currentSocket->write(QString("[Menu Updated]").toUtf8());
//	}
//
//	qDebug() << "SendMenuUpdateMessage Done";
//}

void ServerMainWindow::slotBtnEditClicked() {
	if (_view_Menu->currentIndex().row() == -1)//检查是否选中某行
	{
		QMessageBox::critical(this, "编辑失败", "未选择任何行");
		return;
	}

	DialogEditRecord *dlg = new DialogEditRecord;//初始化订单编辑对话框

	QSqlRecord record = _model->record(_view_Menu->currentIndex().row());//利用QSqlRecord记录一下当前行的的record
	//将当前行的数据提取
	int dishId = record.value(0).toInt();
	QString dishName = record.value(1).toString();
	QString dishType = record.value(2).toString();
	QString dishInfo = record.value(3).toString();
	double dishPrice = record.value(4).toDouble();
	//QString dishPhoto = record.value(5).toString().mid(record.value(5).toString().lastIndexOf("/") + 1, -1);
	QString dishPhoto = record.value(5).toString();
	qDebug() << "dishPhoto:" << dishPhoto;

	// TODO 此处更新菜单

	//赋值一下oldDishType
	oldDishType = dishType;

	dlg->setValue(dishId, dishName, dishType, dishInfo, dishPrice, dishPhoto);//传入值
	dlg->show();


	//利用函数指针调用DialogEditRecord带参数的signalUpdate信号，和ServerMainWindow带参数的slotUpdate槽
	//	void (DialogEditRecord::*pSignalUpdate)(int, QString, QString, QString, QString, QString) = &DialogEditRecord::signalUpdate;
	//	void (ServerMainWindow::*pSlotUpdate)(int, QString, QString, QString, QString, QString) = &ServerMainWindow::slotUpdate;
	//	connect(dlg, pSignalUpdate, this, pSlotUpdate);
	connect(dlg, qOverload<int, const QString, const QString, const QString, const double, const QString, bool>(&DialogEditRecord::signalUpdate),
			this, qOverload<int, const QString, const QString, const QString, const double, const QString, bool>(&ServerMainWindow::slotUpdate));
}

void ServerMainWindow::slotBtnAddClicked() {
	DialogAddRecord *dlg = new DialogAddRecord;

	dlg->setUrl("127.0.0.1");
	dlg->show();

	// TODO 此处更新菜单

	//利用函数指针调用DialogAddRecord带参数的signalSubmit信号，和ServerMainWindow带参数的slotSubmit槽
	void (DialogAddRecord::*pSignalSubmit)(QString, QString, QString, QString, QString) = &DialogAddRecord::signalSubmit;
	void (ServerMainWindow::*pSlotSubmit)(QString, QString, QString, QString, QString) = &ServerMainWindow::slotSubmit;
	connect(dlg, pSignalSubmit, this, pSlotSubmit);
}

void ServerMainWindow::slotBtnDelClicked() {
	if (_view_Menu->currentIndex().row() == -1) {
		QMessageBox::critical(this, "删除失败", "未选择任何行");
		return;
	}

	// TODO 此处更新菜单

	int ret = QMessageBox::question(this, "确认删除", "你确认要删除这个菜品吗？");
	if (ret == QMessageBox::Yes) {
		//先记录一下当前的部分信息
		QSqlRecord record = _model->record(_view_Menu->currentIndex().row());
		QString menuName = record.value(1).toString();
		QString fileName = record.value(5).toString().mid(record.value(5).toString().lastIndexOf("/"), -1);
		QString menuType = record.value(2).toString();

		bool delRet = _model->removeRow(_view_Menu->currentIndex().row());
		if (!delRet) {
			QMessageBox::critical(this, "删除失败", "删除菜品失败！");
			qDebug() << db.lastError().text();
			return;
		}
		bool submitRet = _model->submitAll();
		if (!submitRet) {
			QMessageBox::critical(this, "删除失败", "删除菜品失败！");
			qDebug() << db.lastError().text();
			return;
		}

		// 删除本地和远程的图片
		// TODO 删除本地的图片即可
		QDir dir = QDir::currentPath() + "/Pic";

		if (dir.exists(menuName + ".jpg")) {
			dir.remove(menuName + ".jpg");
			qDebug() << "Delete Menu: remove " + menuName + ".jpg";

			//删除HTTP服务器上的文件
			//HttpFileUpdate updateFile(fileName, _picHost + "/update.php");
			//updateFile.update();
		}
		if (dir.exists(menuName + ".jpeg")) {
			dir.remove(menuName + ".jpeg");
			qDebug() << "Delete Menu: remove " + menuName + ".jpeg";

			//删除HTTP服务器上的文件
			//HttpFileUpdate updateFile(fileName, _picHost + "/update.php");
			//updateFile.update();
		}
		if (dir.exists(menuName + ".png")) {
			dir.remove(menuName + ".png");
			qDebug() << "Delete Menu: remove " + menuName + ".png";

			//删除HTTP服务器上的文件
			//HttpFileUpdate updateFile(fileName, _picHost + "/update.php");
			//updateFile.update();
		}


		//维护menuType表
		// TODO 有menuType的model

		int index = _menuTypeList.indexOf(menuType);

		QSqlQuery query(db);
		if (index != -1) {
			//index++;
			QSqlQuery indexQuery(db);
			qDebug() << indexQuery.exec(tr("SELECT * FROM menuType WHERE TypeName='%1'").arg(menuType));
			indexQuery.next();
			QSqlRecord indexRecord = indexQuery.record();
			qDebug() << indexRecord.value(1).toString();
			index = indexRecord.value(0).toInt();

			int menuTypeNum;
			qDebug() << query.exec(tr("SELECT * FROM menuType WHERE ID=%1").arg(index));
			query.next();
			QSqlRecord record = query.record();
			qDebug() << record.value(1).toString();
			menuTypeNum = record.value(2).toInt();
			qDebug() << menuTypeNum;
			menuTypeNum--;
			qDebug() << menuTypeNum;
			qDebug() << query.exec(tr("UPDATE menuType SET NUM=%1 WHERE ID=%2").arg(menuTypeNum).arg(index));
		}

		//向客户端发送更新菜单消息
		// TODO 发送菜单更新消息
		//emit sendMenuUpdateSignal();
	}
}

void ServerMainWindow::slotSubmit(QString dishName, QString dishType, QString dishInfo, QString dishPrice, QString dishPhoto) {
	qDebug() << "slotSubmit";
	//qDebug()<<dishName<<" "<<dishType<<" "<<dishInfo<<" "<<dishPrice<<" "<<dishPhoto;

	QSqlRecord record = _model->record();
	//插入record信息
	record.setValue(1, dishName);
	record.setValue(2, dishType);
	record.setValue(3, dishInfo);
	record.setValue(4, dishPrice.toDouble());
	// TODO 本地图片目录即可
	//record.setValue(5, _picHost + "/upload/" + dishPhoto);

	bool bRet = _model->insertRecord(-1, record);//插入record

	if (!bRet) {
		QMessageBox::critical(this, "添加失败", "菜品添加失败！");
		qDebug() << "add error " << db.lastError().text();
		return;
	} else
		qDebug() << "insert ok";


	_model->submitAll();


	//维护menuType表
	// TODO 有menuType的model
	int index = _menuTypeList.indexOf(dishType);
	//QSqlRecord currentRecord;

	if (index == -1)//如果原本没有这个菜品种类
	{
		_menuTypeList.append(dishType);//增加到_menuTypeList里面


		QSqlQuery query(db);
		qDebug() << query.exec(tr("INSERT INTO menuType (ID, TypeName, Num) VALUES(NULL,'%1',1)").arg(dishType));


		//给客户端发送更新菜单消息
		//注意一下发送消息的顺序,要先更显完菜品信息才发送更新信息
		// TODO 发送菜单更新消息
		//emit sendMenuUpdateSignal();

	} else {
		//如果原来有这个菜品类
		//菜品数加1
		QSqlQuery indexQuery(db);
		qDebug() << indexQuery.exec(tr("SELECT * FROM menuType WHERE TypeName='%1'").arg(dishType));
		indexQuery.next();
		QSqlRecord indexRecord = indexQuery.record();
		qDebug() << indexRecord.value(1).toString();
		index = indexRecord.value(0).toInt();

		QSqlQuery query(db);
		qDebug() << query.exec(tr("SELECT * FROM menuType WHERE ID=%1").arg(index));
		int menuTypeNum;
		query.next();
		QSqlRecord record = query.record();
		qDebug() << record.value(1).toString();
		menuTypeNum = record.value(2).toInt();
		qDebug() << menuTypeNum;
		menuTypeNum++;
		qDebug() << menuTypeNum;
		qDebug() << query.exec(tr("UPDATE menuType SET NUM=%1 WHERE ID=%2").arg(menuTypeNum).arg(index));


		//给客户端发送更新菜单消息
		//注意一下发送消息的顺序,要先更显完菜品信息才发送更新信息
		// TODO 发送菜单更新消息
		//emit sendMenuUpdateSignal();
	}
}

void ServerMainWindow::slotUpdate(int dishId, const QString dishName, const QString dishType, const QString dishInfo, const double dishPrice, const QString dishPhoto, bool photoUpdate) {
	qDebug() << "slotUpdate";
	// TODO 重构菜单更新
	//将更新信息插入record
	QSqlRecord record = _model->record(_view_Menu->currentIndex().row());
	record.setValue(1, dishName);
	record.setValue(2, dishType);
	record.setValue(3, dishInfo);
	record.setValue(4, dishPrice);

	// 构造一项操作
	using namespace DianaJustEat;
	MenuOperation *ope = new MenuOperation(MenuOperationType::UpdateDish, dishName, dishType, dishInfo, dishPrice, dishPhoto, photoUpdate);
	// TODO 判断图片是否更新
	if (photoUpdate) {
		record.setValue(5, dishPhoto);
	}

	bool bRet = _model->setRecord(_view_Menu->currentIndex().row(), record);
	_operations.push_back(ope);

	if (!bRet) {
		QMessageBox::critical(this, "修改失败", "菜品修改失败！");
		qDebug() << "update error " << db.lastError().text();
		return;
	} else
		qDebug() << "update ok";

	if (!_model->submitAll()) {
		qDebug() << "更新菜单失败";
		qDebug() << _model->lastError().text();
	}
	qDebug() << "_model submitted";

	// 维护menuType表
	// TODO 有menuType的model
	if (oldDishType != dishType) {
		qDebug() << "菜品种类更换" << dishName << dishType << oldDishType;
		if (_menuTypeList.contains(dishType)) {
			// 更换到已有种类
			// TODO 维护MenuType model
			int oldMenuTypeNum;
			int newMenuTypeNum;
			for (int i = 0; i < _menuTypeModel->rowCount(); ++i) {
				auto currentMenuTypeRecord = _menuTypeModel->record(i);
				QString currentMenuTypeName = currentMenuTypeRecord.value(1).toString();

				// oldDishType减一
				if (currentMenuTypeName == oldDishType) {
					currentMenuTypeRecord.setValue(2, currentMenuTypeRecord.value(2).toInt() - 1);
					oldMenuTypeNum = currentMenuTypeRecord.value(2).toInt();

					_menuTypeModel->setRecord(i, currentMenuTypeRecord);
				}
				// 新类型+1
				if (currentMenuTypeName == dishType) {
					currentMenuTypeRecord.setValue(2, currentMenuTypeRecord.value(2).toInt() + 1);
					newMenuTypeNum = currentMenuTypeRecord.value(2).toInt();

					_menuTypeModel->setRecord(i, currentMenuTypeRecord);
				}
			}

			// TODO 添加Operations到操作列表
			qDebug() << oldDishType << oldMenuTypeNum << dishType << newMenuTypeNum;
			MenuTypeOperation *minusOperation = new MenuTypeOperation(MenuTypeOperationType::UpdateType, oldDishType, oldMenuTypeNum);// 旧类型
			MenuTypeOperation *addOperation = new MenuTypeOperation(MenuTypeOperationType::UpdateType, dishType, newMenuTypeNum);     // 新类型
			_operations.push_back(addOperation);
			_operations.push_back(minusOperation);

			_menuTypeNumHash[dishType] = _menuTypeNumHash[dishType] + 1;
			_menuTypeNumHash[oldDishType] = _menuTypeNumHash[oldDishType] - 1;
		} else {
			// 更换到新种类
			// TODO 维护MenuType model
			// 旧种类-1
			int oldMenuTypeNum;
			for (int i = 0; i < _menuTypeModel->rowCount(); ++i) {
				auto currentMenuTypeRecord = _menuTypeModel->record(i);
				QString currentMenuTypeName = currentMenuTypeRecord.value(1).toString();

				if (currentMenuTypeName == oldDishType) {
					currentMenuTypeRecord.setValue(2, currentMenuTypeRecord.value(2).toInt() - 1);
					oldMenuTypeNum = currentMenuTypeRecord.value(2).toInt();

					_menuTypeModel->setRecord(i, currentMenuTypeRecord);
					break;
				}
			}

			// 添加新种类
			QSqlRecord newMenuTypeRecord = _menuTypeModel->record();
			newMenuTypeRecord.setNull(0);
			newMenuTypeRecord.setValue(1, dishType);
			newMenuTypeRecord.setValue(2, 1);// 新种类仅有1样菜品
			_menuTypeModel->insertRecord(_menuTypeModel->rowCount(), newMenuTypeRecord);

			// TODO 添加Operations到操作列表
			MenuTypeOperation *newOperation = new MenuTypeOperation(MenuTypeOperationType::AddType, dishType, 1);                     // 添加新类型
			MenuTypeOperation *addOperation = new MenuTypeOperation(MenuTypeOperationType::UpdateType, dishType, 1);                  // 新类型+1
			MenuTypeOperation *minusOperation = new MenuTypeOperation(MenuTypeOperationType::UpdateType, oldDishType, oldMenuTypeNum);// 旧类型
			_operations.push_back(newOperation);
			_operations.push_back(addOperation);
			_operations.push_back(minusOperation);

			_menuTypeList.append(dishType);
			_menuTypeNumHash[dishType] = 1;
			_menuTypeNumHash[oldDishType] = _menuTypeNumHash[oldDishType] - 1;
		}

		// TODO 提交menuType Model
		if (!_menuTypeModel->submitAll()) {
			qDebug() << "更新菜单失败";
			qDebug() << _menuTypeModel->lastError().text();
		}
		qDebug() << "_menuTypeModel Submitted";
	}

	// 生成菜单更新消息
	generateUpdatedMenu();

	//	if (oldDishType != dishType)//如果菜品种类改变
	//	{
	//		qDebug() << "更换类型";
	//		int index = _menuTypeList.indexOf(oldDishType);
	//		if (index != -1) {
	//			//将旧的菜品种类数减1
	//			QSqlQuery indexQuery(db);
	//			qDebug() << indexQuery.exec(tr("SELECT * FROM menuType WHERE TypeName='%1'").arg(oldDishType));
	//			indexQuery.next();
	//			QSqlRecord indexRecord = indexQuery.record();
	//			qDebug() << indexRecord.value(1).toString();
	//			index = indexRecord.value(0).toInt();
	//
	//			QSqlQuery query(db);
	//			int menuTypeNum;
	//			qDebug() << query.exec(tr("SELECT * FROM menuType WHERE ID=%1").arg(index));
	//			query.next();
	//			QSqlRecord record = query.record();
	//			qDebug() << record.value(1).toString();
	//			menuTypeNum = record.value(2).toInt();
	//			qDebug() << menuTypeNum;
	//			menuTypeNum--;
	//			qDebug() << menuTypeNum;
	//			qDebug() << query.exec(tr("UPDATE menuType SET NUM=%1 WHERE ID=%2").arg(menuTypeNum).arg(index));
	//		}
	//	}
	//
	//	int index = _menuTypeList.indexOf(dishType);
	//	//QSqlRecord currentRecord;
	//
	//	if (index == -1)//如果原本没有这个菜品种类
	//	{
	//
	//		_menuTypeList.append(dishType);//增加到_menuTypeList里面
	//
	//		QSqlQuery query(db);
	//		qDebug() << query.exec(tr("INSERT INTO menuType (ID, TypeName, Num) VALUES(NULL,'%1',1)").arg(dishType));
	//	} else {
	//		//将菜品种类数加1
	//		QSqlQuery indexQuery(db);
	//		qDebug() << indexQuery.exec(tr("SELECT * FROM menuType WHERE TypeName='%1'").arg(dishType));
	//		indexQuery.next();
	//		QSqlRecord indexRecord = indexQuery.record();
	//		qDebug() << indexRecord.value(1).toString();
	//		index = indexRecord.value(0).toInt();
	//
	//		QSqlQuery query(db);
	//		qDebug() << query.exec(tr("SELECT * FROM menuType WHERE ID=%1").arg(index));
	//		int menuTypeNum;
	//		query.next();
	//		QSqlRecord record = query.record();
	//		qDebug() << record.value(1).toString();
	//		menuTypeNum = record.value(2).toInt();
	//		qDebug() << menuTypeNum;
	//		menuTypeNum++;
	//		qDebug() << menuTypeNum;
	//		qDebug() << query.exec(tr("UPDATE menuType SET NUM=%1 WHERE ID=%2").arg(menuTypeNum).arg(index));
	//	}
}

//重写退出事件
void ServerMainWindow::closeEvent(QCloseEvent *event) {
	int ret = QMessageBox::question(this, "确认取消", "您确认要退出服务端吗？");

	if (ret == QMessageBox::Yes) {
		event->accept();
	} else
		event->ignore();
}

// 配置信息保存
void ServerMainWindow::slotUpdateBtnClicked() {
	if (le_TcpHost->text().isEmpty() || le_TcpPort->text().isEmpty() || le_TcpStatusPort->text().isEmpty() || le_ClearShot->text().isEmpty()) {
		QMessageBox::critical(this, "错误", "关键信息不完整！");
		return;
	}

	// 更新信息
	_tcpHost = le_TcpHost->text();
	_tcpPort = le_TcpPort->text().toInt();
	_tcpStatusPort = le_TcpStatusPort->text().toInt();
	_clearShot = le_ClearShot->text().toInt();

	// 保存配置
	using Json = nlohmann::json;
	Json settingJson;
	settingJson["tcpHost"] = _tcpHost.toStdString();
	settingJson["tcpPort"] = _tcpPort;
	settingJson["tcpStatusPort"] = _tcpStatusPort;
	settingJson["clearShot"] = _clearShot;

	QFile settingFile("config.json");
	if (settingFile.open(QFile::WriteOnly)) {
		QByteArray settingData = QString::fromStdString(settingJson.dump(2)).toUtf8();
		settingFile.write(settingData);
		QMessageBox::information(this, "保存成功", "已成功保存配置！");
	} else {
		qDebug() << "打开配置文件失败";
		QMessageBox::critical(this, "保存配置失败", "保存配置失败，无法打开配置文件！");
	}
}

void ServerMainWindow::slotRevBtnClicked() {
	// 重构恢复设置
	le_TcpHost->setText(_tcpHost);
	le_TcpPort->setText(QString::number(_tcpPort));
	le_TcpStatusPort->setText(QString::number(_tcpStatusPort));
	le_ClearShot->setText(QString::number(_clearShot));
}

void ServerMainWindow::slotBtnHistoryClicked() {
	DialogHistoryViewer *dlg = new DialogHistoryViewer;

	dlg->exec();
}

void ServerMainWindow::slotBtnReHandleClicked() {
	if (_table_Orders->currentIndex().row() == -1)//判断是否选中一行
	{
		QMessageBox::critical(this, "处理失败", "未选中任何行!");
		return;
	}

	if (_table_Orders->item(_table_Orders->currentIndex().row(), 0)->text() == "未处理")//判断该订单是否已处理
	{
		QMessageBox::critical(this, "处理失败", "该订单未处理!");
		return;
	}

	int currentRow = _table_Orders->currentRow();//获取一下当前行
	int ret = QMessageBox::question(this, "请求确认", tr("您确认要重新处理该订单吗？\n该订单的订单号: %1\n桌号: %2").arg(_table_Orders->item(currentRow, 1)->text()).arg(_table_Orders->item(currentRow, 2)->text()));
	if (ret == QMessageBox::Yes) {
		_table_Orders->setItem(currentRow, 0, new QTableWidgetItem("未处理"));

		//更新订单统计信息
		_OrdersNoCount++;//未处理订单数加1

		//更新状态栏
		labelOrdersNoCount->setText(tr("未处理订单数：%1").arg(QString::number(_OrdersNoCount)));
	}
}

void ServerMainWindow::slotBtnHandleClicked() {
	/* 订单处理
     * 先将该订单状态修改
     * 更新状态栏订单信息
     * 将该订单存入本地历史菜单数据库
     * 设定时间后删除该订单
    */

	if (_table_Orders->currentIndex().row() == -1)//判断是否已经选中一行
	{
		QMessageBox::critical(this, "处理失败", "未选中任何行!");
		return;
	}

	if (_table_Orders->item(_table_Orders->currentIndex().row(), 0)->text() != "未处理")//判断该订单是否已经处理
	{
		QMessageBox::critical(this, "处理失败", "该订单已经处理!");
		return;
	}

	int currentRow = _table_Orders->currentRow();//获取一下当前订单的总行数

	/* 暴力做法，现在就先存一下该栏信息,后面遍历匹配删除 */
	QTableWidgetItem *currentItemOrderNum = _table_Orders->item(currentRow, 1);

	int ret = QMessageBox::question(this, "请求确认", tr("您确认要处理该订单吗？\n该订单的订单号: %1\n桌号: %2").arg(_table_Orders->item(currentRow, 1)->text()).arg(_table_Orders->item(currentRow, 2)->text()));
	if (ret == QMessageBox::Yes) {
		_table_Orders->setItem(currentRow, 0, new QTableWidgetItem(QIcon(":/Res/ok.ico"), "订单已处理"));//修改订单状态

		//更新订单统计信息
		_OrdersNoCount--;//未处理订单数减1

		//更新状态栏
		labelOrdersNoCount->setText(tr("未处理订单数：%1").arg(QString::number(_OrdersNoCount)));


		//添加该订单到本地数据库
		//建立数据连接
		QSqlDatabase tmpDb = QSqlDatabase::addDatabase("QSQLITE", "tmpSqlite");
		//设置数据库文件名
		QString dbPath = QDir::currentPath() + "/" + "orders.db";
		tmpDb.setDatabaseName(dbPath);

		if (tmpDb.open()) {
			qDebug() << "sqlite数据库连接成功";
		} else
			qDebug() << "sqlite数据库连接失败" << tmpDb.lastError().text();

		QSqlQuery tmpQuery(tmpDb);
		//使用sql的insert语句插入数据
		bool rret = tmpQuery.exec(tr("INSERT INTO Orders VALUES(NULL,'%1','%2',%3,'%4','%5');").arg(_table_Orders->item(currentRow, 1)->text()).arg(_table_Orders->item(currentRow, 2)->text()).arg(_table_Orders->item(currentRow, 3)->text()).arg(_table_Orders->item(currentRow, 4)->text()).arg(_table_Orders->item(currentRow, 5)->text()));
		//bool rret = tmpQuery.exec("INSERT INTO Orders VALUES(NULL,'2019053114563100002','A03',125,'[宫保鸡丁:1],[老八小汉堡:2],[扬州炒饭:2],[鱼香  丝:1]','希望能好吃');");
		qDebug() << rret;
		//关闭本地数据库
		tmpDb.close();


		//QTimer 让已处理的订单定时消失
		QTimer *clearTimer = new QTimer(this);


		connect(clearTimer, &QTimer::timeout, [=]() {
			//绑定QTimer的timeout信号，到规定时间开始处理
			for (int i = 0; i <= _table_Orders->rowCount(); i++)//遍历每一行
			{
				if (_table_Orders->item(i, 1) == currentItemOrderNum)//找到对应行，进行删除
				{
					if (_table_Orders->item(i, 0)->text() != "未处理") {
						_table_Orders->removeRow(i);
						break;
						qDebug() << tr("清除第%1行").arg(QString::number(i));
					}
				}
			}
		});
		clearTimer->setSingleShot(true);     //设置为只执行一次
		clearTimer->start(_clearShot * 1000);//设置消失时间
	}
}

void ServerMainWindow::slotBtnViewerClicked() {
	if (_table_Orders->currentIndex().row() == -1)//判断一下是否选中某行订单信息，==-1表示未选中任何行
	{
		QMessageBox::critical(this, "查看失败", "未选中任何行!");
		return;
	}

	DialogOrdersViewer *dlg = new DialogOrdersViewer;//初始化订单信息查看对话框
	bool isHandled = true;                           //isHandled储存当前订单处理状态
	if (_table_Orders->item(_table_Orders->currentIndex().row(), 0)->text() == "未处理") {
		isHandled = false;
		qDebug() << "未处理";
	}
	//传入值
	dlg->setData(isHandled, _table_Orders->item(_table_Orders->currentIndex().row(), 1)->text(), _table_Orders->item(_table_Orders->currentIndex().row(), 2)->text(), _table_Orders->item(_table_Orders->currentIndex().row(), 3)->text(), _table_Orders->item(_table_Orders->currentIndex().row(), 4)->text(), _table_Orders->item(_table_Orders->currentIndex().row(), 5)->text());
	dlg->exec();
}

void ServerMainWindow::generateUpdatedMenu() {
	qDebug() << "生成菜单更新消息";
	// 生成菜单更新JSON并发送菜单更新消息
	QByteArray menuData;
	using Json = nlohmann::json;
	using namespace DianaJustEat;

	Json menuDataJson;
	Json menuArr = Json::array();
	Json menuTypeArr = Json::array();

	// 转化operation表
	for (int i = 0; i < _operations.size(); ++i) {
		auto currentOperation = _operations.at(i);
		switch (currentOperation->kind()) {
			case OperationKind::MenuOperationKind: {
				// 菜品操作
				MenuOperation *currentMenuOperation = dynamic_cast<MenuOperation *>(currentOperation);
				Json currentMenuOperationJson;
				switch (currentMenuOperation->getOpt()) {
					case MenuOperationType::AddDish: {
						// 添加菜品
						currentMenuOperationJson["Opt"] = 0;
						currentMenuOperationJson["Name"] = currentMenuOperation->getDishName().toStdString();
						currentMenuOperationJson["Type"] = currentMenuOperation->getDishType().toStdString();
						currentMenuOperationJson["Info"] = currentMenuOperation->getDishInfo().toStdString();
						currentMenuOperationJson["Price"] = currentMenuOperation->getDishPrice();
						currentMenuOperationJson["Photo"] = currentMenuOperation->getDishPhoto().toStdString();
						break;
					}
					case MenuOperationType::UpdateDish: {
						// 更新菜品信息
						currentMenuOperationJson["Opt"] = 1;
						currentMenuOperationJson["Name"] = currentMenuOperation->getDishName().toStdString();
						currentMenuOperationJson["Type"] = currentMenuOperation->getDishType().toStdString();
						currentMenuOperationJson["Info"] = currentMenuOperation->getDishInfo().toStdString();
						currentMenuOperationJson["Price"] = currentMenuOperation->getDishPrice();
						currentMenuOperationJson["Photo"] = currentMenuOperation->getDishPhoto().toStdString();
						currentMenuOperationJson["PhotoUpdated"] = currentMenuOperation->getDishPhotoUpdated();
						break;
					}
					case MenuOperationType::DeleteDish: {
						// 删除菜品
						currentMenuOperationJson["Opt"] = 2;
						currentMenuOperationJson["Name"] = currentMenuOperation->getDishName().toStdString();
						break;
					}
					default:
						break;
				}

				menuArr.push_back(currentMenuOperationJson);
				break;
			}

			case OperationKind::MenuTypeOperationKind: {
				// 菜品种类操作
				MenuTypeOperation *currentMenuTypeOperation = dynamic_cast<MenuTypeOperation *>(currentOperation);
				Json currentMenuTypeOperationJson;
				switch (currentMenuTypeOperation->getOpt()) {
					case MenuTypeOperationType::AddType: {
						// 添加菜品种类
						currentMenuTypeOperationJson["Opt"] = 0;
						currentMenuTypeOperationJson["Name"] = currentMenuTypeOperation->getTypeName().toStdString();
						break;
					}
					case MenuTypeOperationType::UpdateType: {
						// 更新菜品种类信息
						currentMenuTypeOperationJson["Opt"] = 1;
						currentMenuTypeOperationJson["Name"] = currentMenuTypeOperation->getTypeName().toStdString();
						currentMenuTypeOperationJson["Num"] = currentMenuTypeOperation->getTypeNum();
						break;
					}
					case MenuTypeOperationType::DeleteType: {
						// 删除菜品种类
						currentMenuTypeOperationJson["Opt"] = 2;
						currentMenuTypeOperationJson["Name"] = currentMenuTypeOperation->getTypeName().toStdString();
						break;
					}
					default:
						break;
				}

				menuTypeArr.push_back(currentMenuTypeOperationJson);
				break;
			}

			default:
				break;
		}
	}

	menuDataJson["code"] = 1;
	menuDataJson["menu"] = menuArr;
	menuDataJson["menuType"] = menuTypeArr;

	menuData = QString::fromStdString(menuDataJson.dump(2)).toUtf8();
	qDebug() << QString::fromStdString(menuDataJson.dump(2));

	tcpServer->sendUpdatedMenu(menuData);

	// 清除操作列表
	_operations.clear();
}
