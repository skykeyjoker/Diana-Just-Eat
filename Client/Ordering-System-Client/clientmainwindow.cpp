#include "clientmainwindow.h"
#include "./ui_clientmainwindow.h"

ClientMainWindow::ClientMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientMainWindow)
{
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
    QHBoxLayout *lay_btn = new QHBoxLayout;
    lay_btn->addStretch(5);
    lay_btn->addWidget(btn_addToCart);
    lay_btn->addStretch(5);

    cartWidget->addWidget(lb_pic);
    cartWidget->addLayout(lay_dishName);
    cartWidget->addLayout(lay_dishPrice);
    cartWidget->addLayout(lay_dishInfo);
    cartWidget->addStretch(2);
    cartWidget->addLayout(lay_btn);
    cartWidget->addStretch(2);

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
    DialogSettings dlg(_dbHost,_dbName,_dbUser,_dbPasswd,_dbPort,_tcpHost,_tcpPort,_picHost);
    dlg.exec();
}

void ClientMainWindow::loadMenu()
{
    qDebug()<<"Load menu...";

    //清除之前的数据


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
}
