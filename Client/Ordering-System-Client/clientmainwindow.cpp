#include "clientmainwindow.h"
#include "./ui_clientmainwindow.h"

ClientMainWindow::ClientMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientMainWindow)
{
    this->setWindowIcon(QIcon(":/Res/icon.png"));
    this->setWindowTitle("自主订餐系统客户端");

    ui->setupUi(this);

    connect(this,&ClientMainWindow::signalAddAlreadyDownloadMenuCount,this,&ClientMainWindow::slotAddAlreadyDownloadMenuCount);

    //连接数据库，读取配置文件
    connectDb();


    //新键Tcp客户端，连接Tcp服务端

    client = new TcpClient;
    client->establishConnect(_tcpHost,_tcpPort);

    connect(client,&TcpClient::signalUpdateMenu,this,&ClientMainWindow::slotUpdateMenu);

    //获取程序图片缓存目录
    _picPath=QDir(QDir::currentPath()+"/Pic");

    /* 初始化界面 */
    QHBoxLayout *mainLay = new QHBoxLayout(ui->centralwidget);


    //_menuList = new QListWidget;


    _viewGroupBox = new QGroupBox("菜品信息");
    QVBoxLayout *cartWidget = new QVBoxLayout(_viewGroupBox);
    lb_pic = new QLabel;
    lb_pic->setMaximumSize(QSize(600,400));
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
    btn_addToCart->setIconSize(QSize(40,40));
    QHBoxLayout *lay_btn = new QHBoxLayout;
    lay_btn->addStretch(5);
    lay_btn->addWidget(btn_addToCart);
    lay_btn->addStretch(5);

    btn_cart = new MyButton(":/Res/shoppingcart.png",QSize(65,65));
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

    cartWidget->setStretchFactor(lb_pic,3);
    cartWidget->setStretchFactor(lay_dishName,1);
    cartWidget->setStretchFactor(lay_dishPrice,1);
    cartWidget->setStretchFactor(lay_dishInfo,2);
    cartWidget->setStretchFactor(lay_btn,1);



    mainLay->addWidget(_menuList);
    mainLay->addWidget(_viewGroupBox);

    mainLay->setStretchFactor(_menuList,3);
    mainLay->setStretchFactor(_viewGroupBox,2);


    //关联菜单列表选中信号与槽
    void (QListWidget::*pSignalItemClicked)(QListWidgetItem *) = &QListWidget::itemClicked;
    void (ClientMainWindow::*pSlotItemClicked)(QListWidgetItem *) = &ClientMainWindow::slotItemClicked;
    connect(_menuList,pSignalItemClicked,this,pSlotItemClicked);

    //关联添加购物车按钮
    connect(btn_addToCart,&QPushButton::clicked,this,&ClientMainWindow::slotAddtoCart);
    connect(btn_addToCart,&QPushButton::clicked,btn_cart,&MyButton::showAddAnimation);
    //关联购物车按钮
    connect(btn_cart,&QPushButton::clicked,this,&ClientMainWindow::slotCartBtnClicked);

        /*  connect(ui->pushButton,&QPushButton::clicked,[=](){

        QStringList strlist;
        strlist<<ui->lineTable->text()<<";"<<ui->linePrice->text()<<";"<<ui->lineMenu->text()<<";"<<ui->lineNote->text();

        QString str;
        foreach(QString s,strlist)
        {
            str+=s;
        }
        qDebug()<<str;
        client->sendData(str.toUtf8());
    });*/



    /* 状态栏信息 */
    lb_cartNumCount = new QLabel(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)),this);
    lb_cartPriceCount = new QLabel(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)),this);
    lb_currentTime = new QLabel(getFormatTimeStamp("yyyy-MM-dd hh:mm:ss"),this);

    //添加到状态栏
    ui->statusbar->addPermanentWidget(lb_cartNumCount);
    ui->statusbar->addPermanentWidget(lb_cartPriceCount);
    ui->statusbar->addPermanentWidget(lb_currentTime);

    //实时时间更新
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,[=](){
        lb_currentTime->setText(getFormatTimeStamp("yyyy-MM-dd hh:mm:ss"));
    });
    timer->start(1000);
}

ClientMainWindow::~ClientMainWindow()
{
    delete ui;
}

bool ClientMainWindow::connectDb()
{
    ReadJson dbInfo;
    bool isOk = dbInfo.readFromFile("./config.json");

    if (!isOk)
    {
        qDebug() << "无法读取配置文件";
        QMessageBox::critical(this, "启动失败", "服务端启动失败，无法读取配置文件。");
        return false;
    }

    //赋值服务器信息
    _dbHost = dbInfo.getDbHost();
    _dbName = dbInfo.getDbName();
    _dbUser = dbInfo.getDbUser();
    _dbPasswd = dbInfo.getDbPasswd();
    _dbPort = dbInfo.getDbPort();

    //赋值图片HTTP服务器信息
    _picHost = dbInfo.getPicHost();

    //赋值tcp服务端信息
    _tcpHost = dbInfo.getTcpHost();
    _tcpPort = dbInfo.getTcpPort();

    //桌号
    _tableNum = dbInfo.getTableNum();

    db.setHostName(_dbHost);
    db.setDatabaseName(_dbName);
    db.setUserName(_dbUser);
    db.setPassword(_dbPasswd);
    db.setPort(_dbPort);

    bool dbRet = db.open();

    if (!dbRet)
    {
        qDebug() << "数据库连接失败 " << db.lastError().text();
        return false;
    }
    else
        qDebug() << "数据库连接成功";


    /*菜单信息显示*/
    loadMenu();

    return true;
}

void ClientMainWindow::on_actionSetting_triggered()
{
    DialogSettings dlg(_dbHost,_dbName,_dbUser,_dbPasswd,_dbPort,_tcpHost,_tcpPort,_picHost,_tableNum);
    dlg.exec();
}

void ClientMainWindow::loadMenu()
{
    qDebug()<<"Load menu...";

    ui->statusbar->showMessage("正在更新菜单信息，请稍等..."); //更新一下状态栏消息

    /* 清除之前的数据 */
    if(_menuList->count())
    {
        _menuList->clear();
    }

    if(!_menuTypeList.isEmpty())
    {
        _menuTypeList.clear();
    }

    if(!_menuNameList.isEmpty())
    {
        _menuNameList.clear();
    }

    if(!_menuTypeNumList.isEmpty())
    {
        _menuTypeNumList.clear();
    }

    if(!_menuFileNameList.isEmpty())
    {
        _menuFileNameList.clear();
    }

    //TODO 购物车不清空就能更新信息
    if(!cartLists.isEmpty())  //购物车也要清空一下，防止提交旧的菜品信息
    {
        cartLists.clear();

        _cartNumCount = 0;
        _cartPriceCount = 0;
    }

    _alreadyDownloadMenuCount=0;
    _menuCount=0;



    //先遍历menuType表，记录菜品分类
    QSqlQuery query(db);
    qDebug()<<query.exec("SELECT * FROM menuType WHERE 1");
    int menuTypeCount = query.size();

    qDebug()<<"更新："<<menuTypeCount;
    query.next();

    for(int i=1;i<=menuTypeCount;i++)
    {
        _menuCount+=query.value(2).toInt();

        query.next();
    }

    query.first();

    for(int i=1;i<=menuTypeCount;i++)
    {
        QString menuTypeName = query.value(1).toString();
        qDebug()<<menuTypeName;
        int menuTypeNum = query.value(2).toInt();
        _menuTypeList.append(menuTypeName);
        _menuTypeNumList.append(menuTypeNum);

        _menuList->setIconSize(QSize(150,150));


        QSqlQuery dishQuery(db);
        qDebug()<<dishQuery.exec(tr("SELECT * FROM menu WHERE Type='%1'").arg(menuTypeName));
        qDebug()<<dishQuery.lastError().text();
        qDebug()<<dishQuery.size();
        dishQuery.next();


        for(int j=1;j<=menuTypeNum;j++)
        {
            QString currenFileName = dishQuery.value(1).toString()+dishQuery.value(5).toString().mid(dishQuery.value(5).toString().lastIndexOf("."),-1);
            QString currentDishName = dishQuery.value(1).toString();
            double currentDishPrice = dishQuery.value(4).toDouble();
            QString currentDishInfo = dishQuery.value(3).toString();

            QString currentList = tr("%1\t%2 RMB\n\n%3").arg(currentDishName).arg(QString::number(currentDishPrice)).arg(currentDishInfo);
            _menuNameList.append(currentList);
            _menuFileNameList.append(currenFileName);

            QString url = dishQuery.value(5).toString();
            qDebug()<<"currenFileName:"<<currenFileName;


            //删除缓存并下载
            if(_picPath.exists(currenFileName)) //先删除缓存
            {
                _picPath.remove(currenFileName);
            }

            HttpFileDownload *picDownload = new HttpFileDownload(url,currenFileName);

            connect(picDownload,&HttpFileDownload::signalDownloadFinished,[=](){

                qDebug()<<"download finished...";

                emit signalAddAlreadyDownloadMenuCount();

            });

            dishQuery.next();
        }


        query.next();
    }

}

void ClientMainWindow::slotUpdateMenu()
{
    loadMenu();
}

void ClientMainWindow::slotItemClicked(QListWidgetItem *item)
{
    qDebug()<<"item clicked";
    QString itemStr = item->text();
    QString dishName = itemStr.mid(0,itemStr.indexOf("\t"));
    QString dishPrice = itemStr.mid(itemStr.indexOf("\t")+1,itemStr.indexOf("\n")-itemStr.indexOf("\t")-1);
    QString dishInfo = itemStr.mid(itemStr.lastIndexOf("\n")+1,-1);
    QString dishPhotoFileName;

    if(_picPath.exists(dishName+".png"))
    {
       dishPhotoFileName = _picPath.path()+"/"+dishName+".png";
    }
    if(_picPath.exists(dishName+".jpg"))
    {
        dishPhotoFileName = _picPath.path()+"/"+dishName+".jpg";
    }
    if(_picPath.exists(dishName+".jpeg"))
    {
        dishPhotoFileName = _picPath.path()+"/"+dishName+".jpeg";
    }
    //QString dishPhotoFileName = _picPath.path()+"/"+_menuFileNameList.at(_menuNameList.indexOf(dishName));
    qDebug()<<dishName;
    qDebug()<<dishPrice;
    qDebug()<<dishInfo;
    qDebug()<<dishPhotoFileName;
    qDebug()<<_menuNameList;

    lb_pic->setPixmap(QPixmap(dishPhotoFileName));
    lb_dishNameContent->setText(dishName);
    lb_dishPriceContent->setText(dishPrice);
    tb_dishInfo->setText(dishInfo);
}

void ClientMainWindow::slotAddAlreadyDownloadMenuCount()
{
    _alreadyDownloadMenuCount++;

    qDebug()<<"alreadyDownloadMenuTypeCount:"<<_alreadyDownloadMenuCount;


    if(_alreadyDownloadMenuCount==_menuCount)
    {
        insertItems();
    }
}

void ClientMainWindow::insertItems()
{
    int currentDishCount = 0;

    qDebug()<<"_menuList->count"<<_menuList->count();
    qDebug()<<"_menuTypeList.size"<<_menuTypeList.size();
    qDebug()<<"_menuTypeNumList.size"<<_menuTypeNumList.size();
    qDebug()<<"_menuNameList.size"<<_menuNameList.size();
    qDebug()<<"_menuFileNameList.size"<<_menuFileNameList.size();


    for(int i=0;i<_menuTypeList.size();i++) //按菜的种类来
    {
        //先插入菜种类头
        QListWidgetItem *currentParentItem = new QListWidgetItem;
        currentParentItem->setText(tr("%1\t共%2种菜品").arg(_menuTypeList.at(i)).arg(QString::number(_menuTypeNumList.at(i))));
        _menuList->addItem(currentParentItem);

        //再插入子类
        for(int j=0;j<_menuTypeNumList.at(i);j++)
        {
            QListWidgetItem *currentChildItem = new QListWidgetItem;
            currentChildItem->setIcon(QIcon(_picPath.path()+"/"+_menuFileNameList.at(currentDishCount)));
            currentChildItem->setText(_menuNameList.at(currentDishCount));

            _menuList->addItem(currentChildItem);

            currentDishCount++;
        }

    }

    ui->statusbar->showMessage("菜单更新成功！",2000); //状态栏更新一下消息
}

void ClientMainWindow::slotAddtoCart()
{
    if(_menuList->currentIndex().row()==-1)
    {
        QMessageBox::critical(this,"添加到购物车失败","请选择菜品后再添加！");
        return;
    }

    if(lb_pic->pixmap()->isNull()||_menuList->currentItem()->icon().isNull())
    {
        QMessageBox::critical(this,"添加到购物车失败","请选择菜品后再添加！");
        return;
    }

    QString currentItemName = lb_dishNameContent->text();
    QString currentItemPrice = lb_dishPriceContent->text().mid(0,lb_dishPriceContent->text().indexOf("R")-1);
    qDebug()<<currentItemName;
    qDebug()<<currentItemPrice;

    //状态栏显示一个添加菜品的临时消息
    ui->statusbar->showMessage(tr("添加1份%1到购物车").arg(currentItemName),1500);

    //先判断购物车是否已经添加该菜品
    for(int i=0;i<cartLists.size();i++)
    {
        if(cartLists.at(i).getItemName()==currentItemName)
        {
            _cartNumCount++;
            const_cast<CartItem&>(cartLists.at(i)).addItem();  //调用必须显式转换
            _cartPriceCount+=currentItemPrice.toDouble();

            lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
            lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));

            return;
        }
    }


    //创建一个临时的CartItem对象
    CartItem currentItem(currentItemName,currentItemPrice.toDouble());
    currentItem.addItem(); //加一个值

    //添加到list
    cartLists.append(currentItem);

    //更新状态栏信息
    _cartNumCount++;
    _cartPriceCount+=currentItem.getSum();

    lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
    lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));
}

void ClientMainWindow::slotCartBtnClicked()
{
    btn_cart->showAddAnimation(); //先放个动画

    DialogCartView *dlg = new DialogCartView(cartLists);
    dlg->show();

    //连接购物车查看界面的信号
    void (DialogCartView::*pSignalCartChanged)(QList<CartItem>) = &DialogCartView::signalCartChanged;
    void (ClientMainWindow::*pSlotCartChanged)(QList<CartItem>) = &ClientMainWindow::slotCartChanged;
    connect(dlg,pSignalCartChanged,this,pSlotCartChanged);
    connect(dlg,&DialogCartView::signalCartCleaned,this,&ClientMainWindow::slotCartCleaned);
    connect(dlg,&DialogCartView::signalCartCheckOut,this,&ClientMainWindow::slotCartCheckOut);
}

void ClientMainWindow::slotCartChanged(QList<CartItem>changedCart)
{
    qDebug()<<"slotCartChanged";

    //更新cartLists内容
    cartLists.clear();
    cartLists=changedCart;

    //更新状态栏信息
    _cartNumCount = 0;
    _cartPriceCount = 0;

    for(int i=0;i<cartLists.size();i++)
    {
        _cartNumCount+=cartLists.at(i).getNum();
        _cartPriceCount+=cartLists.at(i).getSum();
    }

    lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
    lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));
}

void ClientMainWindow::slotCartCleaned()
{
    qDebug()<<"slotCartCleaned()";
    //更新cartLists内容
    cartLists.clear();

    //更新状态栏信息
    _cartNumCount = 0;
    _cartPriceCount = 0;


    lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
    lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));

    //状态栏显示提醒消息
    ui->statusbar->showMessage("购物车已清空",1500);
}

void ClientMainWindow::slotCartCheckOut()
{
    DialogCheckOut *dlg = new DialogCheckOut(cartLists,_cartNumCount,_cartPriceCount);
    dlg->show();

    void (DialogCheckOut::*pSignalReadyCheckOut)(QString) = &DialogCheckOut::signalReadyCheckOut;
    void (ClientMainWindow::*pSlotReadyCheckOut)(QString) = &ClientMainWindow::slotReadyCheckOut;
    connect(dlg,pSignalReadyCheckOut,this,pSlotReadyCheckOut);
}

void ClientMainWindow::slotReadyCheckOut(QString note)
{
    qDebug()<<"slotReadyCheckOut";

/*
        QStringList strlist;
        strlist<<ui->lineTable->text()<<";"<<ui->linePrice->text()<<";"<<ui->lineMenu->text()<<";"<<ui->lineNote->text();

        QString str;
        foreach(QString s,strlist)
        {
            str+=s;
        }
        qDebug()<<str;
        client->sendData(str.toUtf8());

A03;125;[宫保鸡丁:1],[老八小汉堡:2],[扬州炒饭:2],[鱼香肉丝:1];希望能好吃。

*/
    QStringList dataList;
    dataList<<_tableNum<<";";
    dataList<<QString::number(_cartPriceCount)<<";";

    QString cartContent;
    for(int i=0; i<cartLists.size(); i++)
    {
        QString currentCartItem;
        if(i!=cartLists.size()-1)
            currentCartItem="["+cartLists.at(i).getItemName()+":"+QString::number(cartLists.at(i).getNum())+"],";
        else
            currentCartItem="["+cartLists.at(i).getItemName()+":"+QString::number(cartLists.at(i).getNum())+"]";
        qDebug()<<currentCartItem;
        cartContent+=currentCartItem;
        qDebug()<<cartContent;
    }
    dataList<<cartContent<<";";
    dataList<<note;
    qDebug()<<dataList;

    QString data;
    foreach(QString s, dataList)
    {
        data+=s;
    }
    qDebug()<<data;
    bool ret = client->sendData(data.toUtf8());
    if(ret == true)
    {
        QMessageBox::information(this,"下单成功","已成功下单！");
        //清空购物车
        cartLists.clear();

        //更新状态栏信息
        _cartNumCount = 0;
        _cartPriceCount = 0;


        lb_cartNumCount->setText(tr("购物车菜品数：%1").arg(QString::number(_cartNumCount)));
        lb_cartPriceCount->setText(tr("购物车总价：%1").arg(QString::number(_cartPriceCount)));

        //状态栏显示提醒消息
        ui->statusbar->showMessage("成功下单，购物车已清空",2000);

    }
    else
    {
        QMessageBox::critical(this,"下单失败","未能成功下单！请检查客户端设置并重新下单。");
    }
}
