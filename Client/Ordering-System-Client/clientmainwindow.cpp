#include "clientmainwindow.h"

#include "./ui_clientmainwindow.h"

ClientMainWindow::ClientMainWindow(QWidget *parent)
	: QMainWindow(parent),
	  ui(new Ui::ClientMainWindow) {
	// 初始化窗口信息
	this->setWindowIcon(QIcon(":/Res/icon.png"));
	this->setWindowTitle("自主订餐系统客户端");

	ui->setupUi(this);

	// 读取配置文件
	loadSetting();

	//新建Tcp客户端，连接Tcp服务端
	client = new TcpClient(_tcpHost, _tcpPort, _tcpStatusPort);
	client->establishConnect();
	//_tcpHost = "127.0.0.1";
	//_tcpPort = 8081;
	//_tcpStatusPort = 8082;
	//client->establishConnect(_tcpHost, _tcpPort, _tcpStatusPort);

	void (TcpClient::*pSignalQueryMenu)(const QByteArray) = &TcpClient::signalQueryMenu;
	void (ClientMainWindow::*pSlotQueryMenu)(const QByteArray) = &ClientMainWindow::slotQueryMenu;
	connect(client, pSignalQueryMenu, this, pSlotQueryMenu);// 菜单请求返回

	void (TcpClient::*pSignalUpdateMenu)(const QByteArray) = &TcpClient::signalUpdateMenu;
	void (ClientMainWindow::*pSlotUpdateMenu)(const QByteArray) = &ClientMainWindow::slotUpdateMenu;
	connect(client, pSignalUpdateMenu, this, pSlotUpdateMenu);// 菜单更新

	connect(client, &TcpClient::signalDisconnectedToServer, this, &ClientMainWindow::slotDisconnectedToServer);// 断连

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
	// 界面初始化统一放在函数内
	initUI();

	/* 加载菜单 */
	loadMenu();
}

ClientMainWindow::~ClientMainWindow() {
	delete ui;
	// 调用deleteLater删除控件内容
}

void ClientMainWindow::loadSetting() {
	// 读取配置文件，获取服务器消息，并从服务器处请求菜单信息
	// JSON读取改变，不再需要封装，直接利用第三方JSON库进行读取
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
		_tcpStatusPort = json["tcpStatusPort"].get<int>();
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
	if (_menuList->count()) {// 清空菜单List控件
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
				// 图片预留
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

	// 使用线程池下载图片
	QThreadPool threadPool;
	threadPool.setMaxThreadCount(4);
	for (int i = 0; i < _dishes.size(); i++) {
		// 判断图片是否需要下载
		if (!_picPath.exists(_dishes[i].getPhoto())) {
			QString currentPhotoUrl = "http://" + _tcpHost + "/" + _dishes[i].getPhoto();
			QString currentPhotoName = _picPath.path() + "/" + _dishes[i].getPhoto();
			threadPool.start(new HttpFileDownload(currentPhotoUrl, currentPhotoName));
		}
	}
	threadPool.waitForDone();

	// 图片下载完成后将图片插入
	for (int i = 0; i < _menuList->count(); i++) {
		if (!_menuTypeList.contains(_menuList->item(i)->toolTip()))// 子类
		{
			_menuList->item(i)->setIcon(QIcon(_picPath.path() + "/" + _dishNameAndFileNameHash[_menuList->item(i)->toolTip()]));
		}
	}

	//ui->statusbar->showMessage("菜单更新成功！", 2000);//状态栏更新一下消息
}

void ClientMainWindow::initUI() {
	// 初始化界面
	QHBoxLayout *mainLay = new QHBoxLayout(centralWidget());

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
	DialogSettings dlg(_tcpHost, _tcpPort, _tcpStatusPort, _tableNum);
	dlg.exec();
}

void ClientMainWindow::slotUpdateMenu(const QByteArray data) {
	qDebug() << "收到菜单更新信息";
	QString dataStr = QString::fromUtf8(data);
	qDebug() << dataStr;

	bool cartUpdate = false;

	Json menuJson = Json::parse(data, nullptr, false);

	auto menuTypeArr = menuJson["menuType"];
	for (int i = 0; i < menuTypeArr.size(); ++i) {
		int opt = menuTypeArr[i]["Opt"].get<int>();
		qDebug() << opt;
		QString currentMenuTypeName = QString::fromStdString(menuTypeArr[i]["Name"].get<std::string>());
		switch (opt) {
			case 0: {
				// 添加新菜品种类
				qDebug() << "Add Menu Type:" << currentMenuTypeName;
				_menuTypeList.push_back(currentMenuTypeName);
				_menuTypeNumHash[currentMenuTypeName] = 0;
				break;
			}
			case 1: {
				// 更新菜品种类信息
				qDebug() << "Update Menu Type:" << currentMenuTypeName;
				int currentMenuTypeNum = menuTypeArr[i]["Num"].get<int>();
				_menuTypeNumHash[currentMenuTypeName] = currentMenuTypeNum;
				break;
			}
			case 2: {
				// 删除菜品种类
				qDebug() << "Remove Menu Type:" << currentMenuTypeName;
				_menuTypeList.removeAll(currentMenuTypeName);
				_menuTypeNumHash.remove(currentMenuTypeName);
				break;
			}
			default:
				break;
		}
	}

	auto menuArr = menuJson["menu"];
	for (int i = 0; i < menuArr.size(); ++i) {
		int opt = menuArr[i]["Opt"].get<int>();
		qDebug() << opt;
		QString currentDishName = QString::fromStdString(menuArr[i]["Name"].get<std::string>());
		switch (opt) {
			case 0: {
				// 添加一道新菜品
				QString currentDishType = QString::fromStdString(menuArr[i]["Type"].get<std::string>());
				QString currentDishInfo = QString::fromStdString(menuArr[i]["Info"].get<std::string>());
				double currentDishPrice = menuArr[i]["Price"].get<double>();
				QString currentDishPhoto = QString::fromStdString(menuArr[i]["Photo"].get<std::string>());
				qDebug() << "Add Dish:" << currentDishName << currentDishType << currentDishInfo << currentDishPrice << currentDishPhoto;
				Dish currentDish(currentDishName, currentDishType, currentDishInfo, currentDishPrice, currentDishPhoto);
				_dishNameAndFileNameHash[currentDishName] = currentDishPhoto;
				_dishes.push_back(currentDish);
				break;
			}
			case 1: {
				// 更新菜品信息
				QString currentDishType = QString::fromStdString(menuArr[i]["Type"].get<std::string>());
				QString currentDishInfo = QString::fromStdString(menuArr[i]["Info"].get<std::string>());
				double currentDishPrice = menuArr[i]["Price"].get<double>();
				QString currentDishPhoto = QString::fromStdString(menuArr[i]["Photo"].get<std::string>());
				qDebug() << "Update Dish:" << currentDishName << currentDishType << currentDishInfo << currentDishPrice << currentDishPhoto;
				Dish currentDish(currentDishName, currentDishType, currentDishInfo, currentDishPrice, currentDishPhoto);
				for (int j = 0; j < _dishes.size(); ++j)// 删除旧的菜品信息
					if (_dishes[j].getName() == currentDishName)
						_dishes.removeAt(j);
				_dishNameAndFileNameHash[currentDishName] = currentDishPhoto;
				_dishes.push_back(currentDish);

				// 判断图片是否需要重新下载
				bool isPhotoNeedUpdated = menuArr[i]["PhotoUpdated"].get<bool>();
				if (isPhotoNeedUpdated) {
					// 删除旧的图片
					_picPath.remove(currentDishPhoto);
				}

				// 购物车信息调整
				if (_cartSet.contains(currentDishName)) {
					cartUpdate = true;
					for (auto &cartItem : _cartLists) {
						if (cartItem.getItemName() == currentDishName) {
							cartItem.setItemPrice(currentDishPrice);
						}
					}
				}
				break;
			}
			case 2: {
				// 删除菜品
				qDebug() << "Remove Dish" << currentDishName;
				for (int j = 0; j < _dishes.size(); ++j)// 删除菜品信息
					if (_dishes[j].getName() == currentDishName)
						_dishes.removeAt(j);
				_dishNameAndFileNameHash.remove(currentDishName);

				// 购物车信息调整
				if (_cartSet.contains(currentDishName)) {
					cartUpdate = true;
					for (int j = 0; j < _cartLists.size(); ++j) {
						if (_cartLists[j].getItemName() == currentDishName) {
							_cartLists.removeAt(j);
						}
					}
					_cartSet.remove(currentDishName);
				}
				break;
			}
			default:
				break;
		}
	}

	// 显示菜单
	showMenu();

	statusBar()->showMessage("菜单更新成功！", 2000);

	// 菜单信息更新后更新购物车信息
	if (cartUpdate) {
		// 更新状态栏信息
		_cartNumCount = 0;
		_cartPriceCount = 0;

		for (int i = 0; i < _cartLists.size(); i++) {
			_cartNumCount += _cartLists.at(i).getNum();
			_cartPriceCount += _cartLists.at(i).getSum();
		}

		lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
		lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));
		QMessageBox::information(this, "菜单更新", "菜单更新，购物车内商品已同步更新！");
	}
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
	//qDebug() << _menuNameList;

	//展示菜品信息
	//lb_pic->setPixmap(QPixmap(dishPhotoFileName));
	lb_pic->setPixmap(item->icon().pixmap(600, 400));
	lb_dishNameContent->setText(dishName);
	lb_dishPriceContent->setText(dishPrice);
	tb_dishInfo->setText(dishInfo);
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

	// 状态栏显示一个添加菜品的临时消息
	statusBar()->showMessage(tr("添加1份%1到购物车").arg(currentItemName), 1500);

	// 先判断购物车是否已经添加该菜品
	if (_cartSet.contains(currentItemName)) {
		_cartNumCount++;
		for (int i = 0; i < _cartLists.size(); i++) {
			if (_cartLists.at(i).getItemName() == currentItemName) {
				const_cast<CartItem &>(_cartLists.at(i)).addItem();//调用必须显式转换
				_cartPriceCount += currentItemPrice.toDouble();

				lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
				lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));

				return;
			}
		}
	}

	// 创建一个临时的CartItem对象
	CartItem currentItem(currentItemName, currentItemPrice.toDouble());
	currentItem.addItem();//加一个值

	// 添加到list
	_cartLists.append(currentItem);
	_cartSet.insert(currentItemName);

	// 更新状态栏信息
	_cartNumCount++;
	_cartPriceCount += currentItem.getSum();

	lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
	lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));
}

void ClientMainWindow::slotCartBtnClicked() {
	btn_cart->showAddAnimation();// 先放个动画

	DialogCartView *dlg = new DialogCartView(_cartLists);
	dlg->show();

	// 连接购物车查看界面的信号
	void (DialogCartView::*pSignalCartChanged)(QList<CartItem>) = &DialogCartView::signalCartChanged;
	void (ClientMainWindow::*pSlotCartChanged)(QList<CartItem>) = &ClientMainWindow::slotCartChanged;
	connect(dlg, pSignalCartChanged, this, pSlotCartChanged);
	connect(dlg, &DialogCartView::signalCartCleaned, this, &ClientMainWindow::slotCartCleaned);
	connect(dlg, &DialogCartView::signalCartCheckOut, this, &ClientMainWindow::slotCartCheckOut);
}

void ClientMainWindow::slotCartChanged(QList<CartItem> changedCart) {
	qDebug() << "slotCartChanged";

	// 更新cartLists内容
	_cartLists.clear();
	_cartLists = changedCart;

	// 更新cartSet
	_cartSet.clear();
	for (const auto &currentCartItem : _cartLists) {
		_cartSet.insert(currentCartItem.getItemName());
	}
	qDebug() << _cartLists.size();
	qDebug() << _cartSet << _cartSet.size();

	// 更新状态栏信息
	_cartNumCount = 0;
	_cartPriceCount = 0;

	for (int i = 0; i < _cartLists.size(); i++) {
		_cartNumCount += _cartLists.at(i).getNum();
		_cartPriceCount += _cartLists.at(i).getSum();
	}

	lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
	lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));
}

void ClientMainWindow::slotCartCleaned() {
	qDebug() << "slotCartCleaned()";
	// 更新cartLists和cartSet内容
	_cartLists.clear();
	_cartSet.clear();

	// 更新状态栏信息
	_cartNumCount = 0;
	_cartPriceCount = 0;


	lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
	lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));

	// 状态栏显示提醒消息
	statusBar()->showMessage("购物车已清空", 1500);
}

void ClientMainWindow::slotCartCheckOut() {
	DialogCheckOut *dlg = new DialogCheckOut(_cartLists, _cartNumCount, _cartPriceCount);
	dlg->show();

	void (DialogCheckOut::*pSignalReadyCheckOut)(QString) = &DialogCheckOut::signalReadyCheckOut;
	void (ClientMainWindow::*pSlotReadyCheckOut)(const QString &) = &ClientMainWindow::slotReadyCheckOut;
	connect(dlg, pSignalReadyCheckOut, this, pSlotReadyCheckOut);
}

void ClientMainWindow::slotReadyCheckOut(const QString &note)//结帐，发送socket信息
{
	qDebug() << "slotReadyCheckOut";
	/*
	{
		"code": 1,
		"Table":"A03",
		"Price":62.0,
		"Carts": [
			{
  				"Name":"青椒肉丝",
  				"Num": 1
			},
			{
  				"Name":"五花肉鸡公煲",
  				"Num": 2
			}
		],
		"Note": "希望能好吃"
	}
	*/
	using Json = nlohmann::json;
	Json jsonValue;
	jsonValue["code"] = 1;
	jsonValue["Table"] = _tableNum.toStdString();
	jsonValue["Note"] = note.toStdString();
	Json cartArr = Json::array();

	double cartPrice = 0;
	for (const auto &cartItem : _cartLists) {
		QString currentCartItemName = cartItem.getItemName();
		int currentCartItemNum = cartItem.getNum();
		cartPrice += cartItem.getSum();

		// 构造结构
		Json currentCartJson;
		currentCartJson["Name"] = currentCartItemName.toStdString();
		currentCartJson["Num"] = currentCartItemNum;

		cartArr.push_back(currentCartJson);
	}

	// 添加进jsonValue
	jsonValue["Carts"] = cartArr;
	jsonValue["Price"] = cartPrice;

	QByteArray sendData = QString::fromStdString(jsonValue.dump()).toUtf8();// 待发送的完整数据
	qDebug() << QString::fromUtf8(sendData);

	// sendData已转为内部函数，发送订单信息变化
	bool ret = client->sendNewOrder(sendData);
	//bool ret = client->sendData(data.toUtf8());//发送socket信息
	if (ret) {
		QMessageBox::information(this, "下单成功", "已成功下单！");
		//清空购物车
		_cartLists.clear();
		_cartSet.clear();

		//更新状态栏信息
		_cartNumCount = 0;
		_cartPriceCount = 0;


		lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
		lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));

		//状态栏显示提醒消息
		statusBar()->showMessage("成功下单，购物车已清空", 2000);

	} else {
		QMessageBox::critical(this, "下单失败", "未能成功下单！请检查客户端设置并重新下单。");
	}
}

void ClientMainWindow::slotDisconnectedToServer() {
	// 处理断连信号
	QMessageBox::critical(this, "连接失败", "与服务器链接断开，请检查网络设置并重启服务器！");
	exit(1);
}