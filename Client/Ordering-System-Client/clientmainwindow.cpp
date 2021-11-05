#include "clientmainwindow.h"

#include "./ui_clientmainwindow.h"

ClientMainWindow::ClientMainWindow(QWidget *parent)
	: QMainWindow(parent),
	  ui(new Ui::ClientMainWindow) {
	// 初始化窗口信息
	this->setWindowIcon(QIcon(":/Res/icon.png"));
	this->setWindowTitle("自主订餐系统客户端");

	ui->setupUi(this);

	connect(this, &ClientMainWindow::signalAddAlreadyDownloadMenuCount, this, &ClientMainWindow::slotAddAlreadyDownloadMenuCount);

	//连接数据库，读取配置文件
	// TODO 改进配置初始化
	//connectDb();
	loadSetting();

	// TODO TCP通信转到子线程
	//新建Tcp客户端，连接Tcp服务端
	client = new TcpClient;
	_tcpHost = "127.0.0.1";
	_tcpPort = 8081;
	_tcpStatusPort = 8082;
	client->establishConnect(_tcpHost, _tcpPort, _tcpStatusPort);

	void (TcpClient::*pSignalQueryMenu)(const QByteArray) = &TcpClient::signalQueryMenu;
	void (ClientMainWindow::*pSlotQueryMenu)(const QByteArray) = &ClientMainWindow::slotQueryMenu;
	connect(client, pSignalQueryMenu, this, pSlotQueryMenu);                               // 菜单请求返回
	connect(client, &TcpClient::signalUpdateMenu, this, &ClientMainWindow::slotUpdateMenu);// TODO 菜单更新信号，改进
	// TODO 断连信号

	//获取程序图片缓存目录
	if (!QDir::current().exists("Pic")) {
		QDir::current().mkdir("Pic");
	} else {
		// 清空Pic目录
		QDir picDir(QDir::currentPath() + "/Pic");
		picDir.removeRecursively();
		QDir::current().mkdir("Pic");
	}
	_picPath = QDir(QDir::currentPath() + "/Pic");


	/* 初始化界面 */
	// TODO 界面初始化统一放在函数内
	initUI();

	/* 加载菜单 */
	loadMenu();
}

ClientMainWindow::~ClientMainWindow() {
	delete ui;
	// 调用deleteLater删除控件内容
}

void ClientMainWindow::loadSetting() {
	// TODO 读取配置文件，获取服务器消息，并从服务器处请求菜单信息
	// TODO JSON读取改变，不再需要封装，直接利用第三方JSON库进行读取
	using Json = nlohmann::json;
	QFile file("config.json");
	if (file.open(QFile::ReadOnly)) {
		Json json;
		QByteArray buf = file.readAll();

		if (buf.isEmpty()) {
			qDebug() << "无法读取配置文件，配置文件为空";
			QMessageBox::critical(this, "启动失败", "服务端启动失败，无法读取配置文件。");
			return;
		}

		json = Json::parse(buf.data(), nullptr, false);// 解析JSON，不抛出异常
		if (json.is_null()) {
			qDebug() << "解析配置文件失败";
			QMessageBox::critical(this, "启动失败", "解析配置文件失败。");
			return;
		}
		_tcpHost = QString::fromStdString(json["tcpHost"].get<std::string>());
		_tcpPort = json["tcpPort"].get<int>();
		_tableNum = QString::fromStdString(json["tableNum"].get<std::string>());

		qDebug() << _tcpHost << _tcpPort << _tableNum;

		file.close();

	} else {
		qDebug() << "无法打开配置文件";
		QMessageBox::critical(this, "启动失败", "服务端启动失败，无法打开配置文件。");
		return;
	}
}

void ClientMainWindow::loadMenu() {
	// 请求菜单
	statusBar()->showMessage("正在更新菜单信息，请稍等...");//更新一下状态栏消息
	client->queryMenu();                                    // 发送请求菜单消息
}

void ClientMainWindow::slotQueryMenu(const QByteArray data) {
	// 收到菜单请求消息返回后，更新菜单
	qDebug() << "收到菜单信息";
	QString dataStr = QString::fromUtf8(data);
	qDebug() << dataStr;

	Json menuJson = Json::parse(data, nullptr, false);

	if (!_dishes.isEmpty())
		_dishes.clear();
	if (_menuTypeList.isEmpty())
		_dishes.clear();
	if (!_menuTypeNumHash.isEmpty())
		_menuTypeNumHash.clear();
	if (!_dishNameAndFileNameHash.isEmpty())
		_dishNameAndFileNameHash.clear();

	auto menuTypeArr = menuJson["menuType"];
	for (int i = 0; i < menuTypeArr.size(); ++i) {
		QString currentDishTypeName = QString::fromStdString(menuTypeArr[i]["Name"].get<std::string>());
		int currentDishTypeNum = menuTypeArr[i]["Num"].get<int>();
		_menuTypeList.push_back(currentDishTypeName);
		_menuTypeNumHash[currentDishTypeName] = currentDishTypeNum;
	}

	auto menuArr = menuJson["menu"];
	for (int i = 0; i < menuArr.size(); ++i) {
		QString currentDishName = QString::fromStdString(menuArr[i]["Name"].get<std::string>());
		QString currentDishType = QString::fromStdString(menuArr[i]["Type"].get<std::string>());
		QString currentDishInfo = QString::fromStdString(menuArr[i]["Info"].get<std::string>());
		double currentDishPrice = menuArr[i]["Price"].get<double>();
		QString currentDishPhoto = QString::fromStdString(menuArr[i]["Photo"].get<std::string>());
		qDebug() << currentDishName << currentDishType << currentDishInfo << currentDishPrice << currentDishPhoto;
		Dish currentDish(currentDishName, currentDishType, currentDishInfo, currentDishPrice, currentDishPhoto);
		_dishNameAndFileNameHash[currentDishName] = currentDishPhoto;
		_dishes.push_back(currentDish);
	}

	qDebug() << _menuTypeList;

	// 显示菜单
	showMenu();

	statusBar()->showMessage("菜单加载成功！", 2000);
}

void ClientMainWindow::showMenu() {
	if (_menuList->count()) {
		_menuList->clear();
	}

	_menuList->setIconSize(QSize(150, 150));

	for (int i = 0; i < _menuTypeList.size(); i++)//按菜的种类来
	{
		//先插入菜种类头
		QListWidgetItem *currentParentItem = new QListWidgetItem;
		currentParentItem->setToolTip(_menuTypeList.at(i));
		//currentParentItem->setText(tr("%1\t共%2种菜品").arg(_menuTypeList.at(i)).arg(QString::number(_menuTypeNumList.at(i))));
		currentParentItem->setText(tr("%1\t共%2种菜品").arg(_menuTypeList.at(i)).arg(QString::number(_menuTypeNumHash[_menuTypeList.at(i)])));
		_menuList->addItem(currentParentItem);

		//再插入子类
		int currentDishCount = 0;
		for (int j = 0; j < _dishes.size(); j++) {
			if (_dishes[j].getType() == _menuTypeList[i]) {
				QListWidgetItem *currentChildItem = new QListWidgetItem;
				currentChildItem->setToolTip(_dishes[j].getName());
				// TODO 图片预留
				qDebug() << _picPath.path() + "/" + _dishes[j].getPhoto();
				currentChildItem->setIcon(QIcon());
				//currentChildItem->setIcon(QIcon(_picPath.path() + "/" + _dishes[j].getPhoto()));
				//currentChildItem->setIcon(QIcon(_picPath.path() + "/" + _menuFileNameList.at(currentDishCount)));

				QString currentDishName = _dishes[j].getName();
				double currentDishPrice = _dishes[j].getPrice();
				QString currentDishInfo = _dishes[j].getInfo();

				QString currentDish = tr("%1\t%2 RMB\n\n%3").arg(currentDishName).arg(QString::number(currentDishPrice)).arg(currentDishInfo);
				currentChildItem->setText(currentDish);

				_menuList->addItem(currentChildItem);

				currentDishCount++;
				if (currentDishCount == _menuTypeNumHash[_menuTypeList[i]])
					break;
			}
		}
	}

	// TODO 使用线程池下载图片
	QThreadPool threadPool;
	threadPool.setMaxThreadCount(4);
	// TODO 重写下载worker，继承自QRunnable
	for (int i = 0; i < _dishes.size(); i++) {
		QString currentPhotoUrl = "http://" + _tcpHost + "/" + _dishes[i].getPhoto();
		QString currentPhotoName = _picPath.path() + "/" + _dishes[i].getPhoto();
		threadPool.start(new HttpFileDownload(currentPhotoUrl, currentPhotoName));
	}
	threadPool.waitForDone();

	// TODO 图片下载完成后将图片插入
	for (int i = 0; i < _menuList->count(); i++) {
		if (!_menuTypeList.contains(_menuList->item(i)->toolTip()))// 子类
		{
			_menuList->item(i)->setIcon(QIcon(_picPath.path() + "/" + _dishNameAndFileNameHash[_menuList->item(i)->toolTip()]));
		}
	}

	//ui->statusbar->showMessage("菜单更新成功！", 2000);//状态栏更新一下消息
}

void ClientMainWindow::initUI() {
	// TODO 初始化界面
	QHBoxLayout *mainLay = new QHBoxLayout(ui->centralwidget);

	_viewGroupBox = new QGroupBox("菜品信息");
	QVBoxLayout *cartWidget = new QVBoxLayout(_viewGroupBox);
	lb_pic = new QLabel;
	lb_pic->setMaximumSize(QSize(600, 400));
	lb_dishName = new QLabel("菜品名：");
	lb_dishNameContent = new QLabel;
	lb_dishPrice = new QLabel("菜品价格:");
	lb_dishPriceContent = new QLabel;
	lb_dishInfo = new QLabel("菜品信息：");
	tb_dishInfo = new QTextBrowser;

	QHBoxLayout *lay_dishName = new QHBoxLayout;
	lay_dishName->addWidget(lb_dishName);
	lay_dishName->addWidget(lb_dishNameContent);

	QHBoxLayout *lay_dishPrice = new QHBoxLayout;
	lay_dishPrice->addWidget(lb_dishPrice);
	lay_dishPrice->addWidget(lb_dishPriceContent);

	QHBoxLayout *lay_dishInfo = new QHBoxLayout;
	lay_dishInfo->addWidget(lb_dishInfo);
	lay_dishInfo->addWidget(tb_dishInfo);

	QPushButton *btn_addToCart = new QPushButton("加入到购物车");
	btn_addToCart->setIcon(QIcon(":/Res/addcart.png"));
	btn_addToCart->setIconSize(QSize(40, 40));
	QHBoxLayout *lay_btn = new QHBoxLayout;
	lay_btn->addStretch(5);
	lay_btn->addWidget(btn_addToCart);
	lay_btn->addStretch(5);

	btn_cart = new MyButton(":/Res/shoppingcart.png", QSize(65, 65));
	QHBoxLayout *lay_cartBtn = new QHBoxLayout;
	lay_cartBtn->addStretch(3);
	lay_cartBtn->addWidget(btn_cart);

	cartWidget->addWidget(lb_pic);
	cartWidget->addLayout(lay_dishName);
	cartWidget->addLayout(lay_dishPrice);
	cartWidget->addLayout(lay_dishInfo);
	cartWidget->addStretch(2);
	cartWidget->addLayout(lay_btn);
	cartWidget->addStretch(2);
	cartWidget->addLayout(lay_cartBtn);

	cartWidget->setStretchFactor(lb_pic, 3);
	cartWidget->setStretchFactor(lay_dishName, 1);
	cartWidget->setStretchFactor(lay_dishPrice, 1);
	cartWidget->setStretchFactor(lay_dishInfo, 2);
	cartWidget->setStretchFactor(lay_btn, 1);

	_menuList = new QListWidget;
	mainLay->addWidget(_menuList);
	mainLay->addWidget(_viewGroupBox);

	mainLay->setStretchFactor(_menuList, 3);
	mainLay->setStretchFactor(_viewGroupBox, 2);

	/* 状态栏信息 */
	lb_cartNumCount = new QLabel(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)), this);
	lb_cartPriceCount = new QLabel(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)), this);
	lb_currentTime = new QLabel(getFormatTimeStamp("yyyy-MM-dd hh:mm:ss"), this);

	//添加到状态栏
	statusBar()->addPermanentWidget(lb_cartNumCount);
	statusBar()->addPermanentWidget(lb_cartPriceCount);
	statusBar()->addPermanentWidget(lb_currentTime);

	//实时时间更新
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, [=]() {
		lb_currentTime->setText(getFormatTimeStamp("yyyy-MM-dd hh:mm:ss"));
	});
	timer->start(1000);

	//关联菜单列表选中信号与槽
	void (QListWidget::*pSignalItemClicked)(QListWidgetItem *) = &QListWidget::itemClicked;
	void (ClientMainWindow::*pSlotItemClicked)(QListWidgetItem *) = &ClientMainWindow::slotItemClicked;
	connect(_menuList, pSignalItemClicked, this, pSlotItemClicked);


	//关联添加购物车按钮
	connect(btn_addToCart, &QPushButton::clicked, this, &ClientMainWindow::slotAddtoCart);
	connect(btn_addToCart, &QPushButton::clicked, btn_cart, &MyButton::showAddAnimation);
	//关联购物车按钮
	connect(btn_cart, &QPushButton::clicked, this, &ClientMainWindow::slotCartBtnClicked);
}

void ClientMainWindow::on_actionSetting_triggered() {
	// TODO 设置重写
	//DialogSettings dlg(_dbHost, _dbName, _dbUser, _dbPasswd, _dbPort, _tcpHost, _tcpPort, _picHost, _tableNum);
	//dlg.exec();
}

//void ClientMainWindow::loadMenu()//加载菜品
//{
//	qDebug() << "Load menu...";
//
//	ui->statusbar->showMessage("正在更新菜单信息，请稍等...");//更新一下状态栏消息
//
//	/* 清除之前的数据 */
//	if (_menuList->count())//菜单列表
//	{
//		_menuList->clear();
//	}
//
//	if (!_menuTypeList.isEmpty())//菜品种类列表
//	{
//		_menuTypeList.clear();
//	}
//
//	if (!_menuNameList.isEmpty())//菜品名列表
//	{
//		_menuNameList.clear();
//	}
//
//	if (!_menuTypeNumList.isEmpty())//菜品种类名列表
//	{
//		_menuTypeNumList.clear();
//	}
//
//	if (!_menuFileNameList.isEmpty())//菜品图片文件名列表
//	{
//		_menuFileNameList.clear();
//	}
//
//	//TODO 购物车不清空就能更新信息
//	if (!cartLists.isEmpty())//购物车也要清空一下，防止提交旧的菜品信息
//	{
//		cartLists.clear();
//
//		_cartNumCount = 0;
//		_cartPriceCount = 0;
//	}
//
//	_alreadyDownloadMenuCount = 0;
//	_menuCount = 0;
//
//	// TODO 架构改变，客户端不再需要进行数据库操作
//	//先遍历menuType表，记录菜品分类
//	QSqlQuery query(db);
//	qDebug() << query.exec("SELECT * FROM menuType WHERE 1");
//	int menuTypeCount = query.size();//菜品种类总共数量
//
//	qDebug() << "更新：" << menuTypeCount;
//	query.next();//必须执行一下.next()让他指向第一条记录，否则记录会指向第一条记录之前的记录
//
//	for (int i = 1; i <= menuTypeCount; i++) {
//		_menuCount += query.value(2).toInt();//_menuCount记录着一共多少菜品（是总菜品数！而不是总菜品种类数！）
//
//		query.next();
//	}
//
//	query.first();//重新指向第一条记录
//
//	for (int i = 1; i <= menuTypeCount; i++) {
//		//添加一种菜品种类
//		QString menuTypeName = query.value(1).toString();
//		qDebug() << menuTypeName;
//		int menuTypeNum = query.value(2).toInt();
//		_menuTypeList.append(menuTypeName);
//		_menuTypeNumList.append(menuTypeNum);
//
//		_menuList->setIconSize(QSize(150, 150));
//
//
//		QSqlQuery dishQuery(db);
//		qDebug() << dishQuery.exec(tr("SELECT * FROM menu WHERE Type='%1'").arg(menuTypeName));
//		qDebug() << dishQuery.lastError().text();
//		qDebug() << dishQuery.size();
//		dishQuery.next();
//
//
//		for (int j = 1; j <= menuTypeNum; j++) {
//			//添加一个菜品
//			QString currenFileName = dishQuery.value(1).toString() + dishQuery.value(5).toString().mid(dishQuery.value(5).toString().lastIndexOf("."), -1);
//			QString currentDishName = dishQuery.value(1).toString();
//			double currentDishPrice = dishQuery.value(4).toDouble();
//			QString currentDishInfo = dishQuery.value(3).toString();
//
//			QString currentList = tr("%1\t%2 RMB\n\n%3").arg(currentDishName).arg(QString::number(currentDishPrice)).arg(currentDishInfo);
//			_menuNameList.append(currentList);
//			_menuFileNameList.append(currenFileName);
//
//			QString url = dishQuery.value(5).toString();
//			qDebug() << "currenFileName:" << currenFileName;
//
//
//			//删除缓存并下载
//			if (_picPath.exists(currenFileName))//先删除缓存
//			{
//				_picPath.remove(currenFileName);
//			}
//
//			HttpFileDownload *picDownload = new HttpFileDownload(url, currenFileName);
//
//			connect(picDownload, &HttpFileDownload::signalDownloadFinished, [=]() {
//				qDebug() << "download finished...";
//
//				emit signalAddAlreadyDownloadMenuCount();//下载完一张图片，就发送一个下载完成消息
//			});
//
//			dishQuery.next();
//		}
//
//
//		query.next();
//	}
//}

void ClientMainWindow::slotUpdateMenu() {
	// TODO 重新定义菜单更新槽函数
	//loadMenu();
}

void ClientMainWindow::slotItemClicked(QListWidgetItem *item)//选择一个菜品，展示菜品详细信息
{
	qDebug() << "item clicked";
	QString itemStr = item->text();
	//分离菜品信息
	QString dishName = itemStr.mid(0, itemStr.indexOf("\t"));
	QString dishPrice = itemStr.mid(itemStr.indexOf("\t") + 1, itemStr.indexOf("\n") - itemStr.indexOf("\t") - 1);
	QString dishInfo = itemStr.mid(itemStr.lastIndexOf("\n") + 1, -1);
	QString dishPhotoFileName;

	//图片信息分离
	if (_picPath.exists(dishName + ".png")) {
		dishPhotoFileName = _picPath.path() + "/" + dishName + ".png";
	}
	if (_picPath.exists(dishName + ".jpg")) {
		dishPhotoFileName = _picPath.path() + "/" + dishName + ".jpg";
	}
	if (_picPath.exists(dishName + ".jpeg")) {
		dishPhotoFileName = _picPath.path() + "/" + dishName + ".jpeg";
	}
	//QString dishPhotoFileName = _picPath.path()+"/"+_menuFileNameList.at(_menuNameList.indexOf(dishName));
	qDebug() << dishName;
	qDebug() << dishPrice;
	qDebug() << dishInfo;
	qDebug() << dishPhotoFileName;
	qDebug() << _menuNameList;

	//展示菜品信息
	//lb_pic->setPixmap(QPixmap(dishPhotoFileName));
	lb_pic->setPixmap(item->icon().pixmap(600, 400));
	lb_dishNameContent->setText(dishName);
	lb_dishPriceContent->setText(dishPrice);
	tb_dishInfo->setText(dishInfo);
}

void ClientMainWindow::slotAddAlreadyDownloadMenuCount() {
	_alreadyDownloadMenuCount++;//已经下载的图片数+1

	qDebug() << "alreadyDownloadMenuTypeCount:" << _alreadyDownloadMenuCount;


	if (_alreadyDownloadMenuCount == _menuCount)//如果图片全部下载完成
	{
		insertItems();//菜品图片全部下载完成后，准备开始添加菜品信息到listwidget中
	}
}

void ClientMainWindow::insertItems() {
	int currentDishCount = 0;

	qDebug() << "_menuList->count" << _menuList->count();
	qDebug() << "_menuTypeList.size" << _menuTypeList.size();
	qDebug() << "_menuTypeNumList.size" << _menuTypeNumList.size();
	qDebug() << "_menuNameList.size" << _menuNameList.size();
	qDebug() << "_menuFileNameList.size" << _menuFileNameList.size();


	for (int i = 0; i < _menuTypeList.size(); i++)//按菜的种类来
	{
		//先插入菜种类头
		QListWidgetItem *currentParentItem = new QListWidgetItem;
		currentParentItem->setText(tr("%1\t共%2种菜品").arg(_menuTypeList.at(i)).arg(QString::number(_menuTypeNumList.at(i))));
		_menuList->addItem(currentParentItem);

		//再插入子类
		for (int j = 0; j < _menuTypeNumList.at(i); j++) {
			QListWidgetItem *currentChildItem = new QListWidgetItem;
			currentChildItem->setIcon(QIcon(_picPath.path() + "/" + _menuFileNameList.at(currentDishCount)));
			currentChildItem->setText(_menuNameList.at(currentDishCount));

			_menuList->addItem(currentChildItem);

			currentDishCount++;
		}
	}

	ui->statusbar->showMessage("菜单更新成功！", 2000);//状态栏更新一下消息
}

void ClientMainWindow::slotAddtoCart() {
	if (_menuList->currentIndex().row() == -1) {
		QMessageBox::critical(this, "添加到购物车失败", "请选择菜品后再添加！");
		return;
	}

	if (lb_pic->pixmap()->isNull() || _menuList->currentItem()->icon().isNull()) {
		QMessageBox::critical(this, "添加到购物车失败", "请选择菜品后再添加！");
		return;
	}

	QString currentItemName = lb_dishNameContent->text();
	QString currentItemPrice = lb_dishPriceContent->text().mid(0, lb_dishPriceContent->text().indexOf("R") - 1);
	qDebug() << currentItemName;
	qDebug() << currentItemPrice;

	//状态栏显示一个添加菜品的临时消息
	ui->statusbar->showMessage(tr("添加1份%1到购物车").arg(currentItemName), 1500);

	//先判断购物车是否已经添加该菜品
	for (int i = 0; i < cartLists.size(); i++) {
		if (cartLists.at(i).getItemName() == currentItemName) {
			_cartNumCount++;
			const_cast<CartItem &>(cartLists.at(i)).addItem();//调用必须显式转换
			_cartPriceCount += currentItemPrice.toDouble();

			lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
			lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));

			return;
		}
	}


	//创建一个临时的CartItem对象
	CartItem currentItem(currentItemName, currentItemPrice.toDouble());
	currentItem.addItem();//加一个值

	//添加到list
	cartLists.append(currentItem);

	//更新状态栏信息
	_cartNumCount++;
	_cartPriceCount += currentItem.getSum();

	lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
	lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));
}

void ClientMainWindow::slotCartBtnClicked() {
	btn_cart->showAddAnimation();//先放个动画

	DialogCartView *dlg = new DialogCartView(cartLists);
	dlg->show();

	//连接购物车查看界面的信号
	void (DialogCartView::*pSignalCartChanged)(QList<CartItem>) = &DialogCartView::signalCartChanged;
	void (ClientMainWindow::*pSlotCartChanged)(QList<CartItem>) = &ClientMainWindow::slotCartChanged;
	connect(dlg, pSignalCartChanged, this, pSlotCartChanged);
	connect(dlg, &DialogCartView::signalCartCleaned, this, &ClientMainWindow::slotCartCleaned);
	connect(dlg, &DialogCartView::signalCartCheckOut, this, &ClientMainWindow::slotCartCheckOut);
}

void ClientMainWindow::slotCartChanged(QList<CartItem> changedCart) {
	qDebug() << "slotCartChanged";

	//更新cartLists内容
	cartLists.clear();
	cartLists = changedCart;

	//更新状态栏信息
	_cartNumCount = 0;
	_cartPriceCount = 0;

	for (int i = 0; i < cartLists.size(); i++) {
		_cartNumCount += cartLists.at(i).getNum();
		_cartPriceCount += cartLists.at(i).getSum();
	}

	lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
	lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));
}

void ClientMainWindow::slotCartCleaned() {
	qDebug() << "slotCartCleaned()";
	//更新cartLists内容
	cartLists.clear();

	//更新状态栏信息
	_cartNumCount = 0;
	_cartPriceCount = 0;


	lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
	lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));

	//状态栏显示提醒消息
	ui->statusbar->showMessage("购物车已清空", 1500);
}

void ClientMainWindow::slotCartCheckOut() {
	DialogCheckOut *dlg = new DialogCheckOut(cartLists, _cartNumCount, _cartPriceCount);
	dlg->show();

	void (DialogCheckOut::*pSignalReadyCheckOut)(QString) = &DialogCheckOut::signalReadyCheckOut;
	void (ClientMainWindow::*pSlotReadyCheckOut)(QString) = &ClientMainWindow::slotReadyCheckOut;
	connect(dlg, pSignalReadyCheckOut, this, pSlotReadyCheckOut);
}

void ClientMainWindow::slotReadyCheckOut(QString note)//结帐，发送socket信息
{
	qDebug() << "slotReadyCheckOut";

	/*
    A03;125;[宫保鸡丁:1],[老八小汉堡:2],[扬州炒饭:2],[鱼香肉丝:1];希望能好吃。
*/
	QStringList dataList;//用一个QStringList来存取要发送的订单socket信息
	dataList << _tableNum << ";";
	dataList << QString::number(_cartPriceCount) << ";";

	QString cartContent;//讲购物车菜品信息格式化合并并存入到datalist
	for (int i = 0; i < cartLists.size(); i++) {
		QString currentCartItem;
		if (i != cartLists.size() - 1)
			currentCartItem = "[" + cartLists.at(i).getItemName() + ":" + QString::number(cartLists.at(i).getNum()) + "],";
		else
			currentCartItem = "[" + cartLists.at(i).getItemName() + ":" + QString::number(cartLists.at(i).getNum()) + "]";
		qDebug() << currentCartItem;
		cartContent += currentCartItem;
		qDebug() << cartContent;
	}
	dataList << cartContent << ";";
	dataList << note;
	qDebug() << dataList;

	QString data;//讲datalist转为整个的QString
	foreach (QString s, dataList) {
		data += s;
	}
	qDebug() << data;

	// TODO sendData已转为内部函数，发送订单信息变化
	bool ret = client->sendNewOrder(data.toUtf8());
	//bool ret = client->sendData(data.toUtf8());//发送socket信息
	if (ret) {
		QMessageBox::information(this, "下单成功", "已成功下单！");
		//清空购物车
		cartLists.clear();

		//更新状态栏信息
		_cartNumCount = 0;
		_cartPriceCount = 0;


		lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
		lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));

		//状态栏显示提醒消息
		ui->statusbar->showMessage("成功下单，购物车已清空", 2000);

	} else {
		QMessageBox::critical(this, "下单失败", "未能成功下单！请检查客户端设置并重新下单。");
	}
}
