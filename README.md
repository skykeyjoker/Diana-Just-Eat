# Ordering-System








## 项目简介

一个基于Qt5实现的自主订餐系统。大量使用了TCP，MySql，Sqlite相关技术，~~支持多平台操作~~（理论上能跑Qt的操作环境就支持）。

项目大体架构如下：

* 服务端实现4个主要功能：

  * **实时**接收客户端订单，并能对订单进行查看、处理和重新处理已经处理的订单。已经处理的订单在**规定时间**后会归档。使用*QTcpServer*和*QTcpSocket*实现服务端与客户端通信。
  * 查看历史订单。历史订单储存在**本地**Sqlite数据库中。使用*QSqlDataBase*等数据库操作封装库实现。
  * 查看、修改菜单信息，支持对菜单进行增、删、查、改操作。为支持服务端、多客户端同时查看菜单信息，将菜单信息储存在**远程**MySql数据库中。使用*QSqlDataBase*等数据库操作封装库以及*QNetworkAccessManager*等封装库实现。
  * 对服务端配置进行修改，如远程Mysql服务端信息，本地Tcp服务端信息。涉及Json文件的读写。使用到了*QJson*等Json文件操作相关封装库。

* 客户端实现4个主要功能

  * 显示菜单，支持添加购物车。
  * 对购物车进行增删
  * 提交订单至服务端
  * 修改客户端相关配置信息，如服务端Tcp服务信息。





项目截图：

（写完再放）





------





## 具体功能实现

### 服务端

服务端主要由三个Tab组成，分别对应着实时订单接收与处理、查看历史订单，查看修改菜单信息，修改服务器配置四个功能。

服务端主要涉及Tcp操作与MySql、Sqlite以及Http操作。服务端使用Tcp实时与客户端进行信息交换；使用远程MySql存取菜单数据；使用本地Sqlite存取历史订单。服务端将图片存在远程Http服务器中，方便客户端与服务端同时访问。

MySql结构如下：

```mysql
.DataBase: Ordering-Sytem
.TABLES: menu, menuType, config
/menu       --菜品数据 
  |-ID-|-Name-|-Type-|-Info-|-Price-|-Photo-|
(ID INT PRIMARY KEY AUTOINCREMENT, Name TEXT, Type TEXT, Info TEXT, Price DOUBLE, Photo Text)
/menuType   --菜品种类数据
  |-ID-|-TypeName-|-Num-|
(ID INT PRIMARY KEY AUTOINCREMENT, TypeName Text, Num Text)
```

Sqlite结构如下：

```mysql
.DataBase: orders.db
.TABLES: orders
/orders   --历史订单
  |-ID-|-OrderNum-|-TableNum-|-OrderPrice-|-OrderContent-|-OrderNote-|
(ID INTEGER PRIMARY KEY AUTOINCREMENT, OrderNum TEXT, OrderTabelNum TEXT, OrderPrice DOUBLE, OrderContent TEXT, OrderNote TEXT)
```

Http服务器目录结构如下：

```
\upload
 ---1.jpg
 ---2.jpg
upload_file.php
update.php
```





#### 实时订单接收与处理

服务端程序启动后，会创建名为*_tcpServer*的Tcp服务端，并使用`_tcpServer->listen(QHostAddress::Any, _tcpPort)`启动监听（*_tcpPort*来自于配置文件）。并通过`connect(_tcpServer,&QTcpServer::newConnection,this,&ServerMainWindow::slotNewConnection)`绑定信号与槽，当有客户端连接时，调用*slotNewConnection*()方法。

 *slotNewConnection()*将会通过`while(_tcpServer->hasPendingConnections())`判断是否有未处理的连接，将新的socket连接存入*QList<QTcpSocket \* >*类型的*_tcpSocket*中。并使用`connect(currentSocket,&QTcpSocket::readyRead,this,&ServerMainWindow::slotReadyRead)`为新的scoket提供槽函数*slotReadyRead()*，来接受数据。

*slotReadyRead()*循环`readAll()`接收数据。数据格式如下：

> A03;125;[宫保鸡丁:1],[老八小汉堡:2],[扬州炒饭:2],[鱼香肉丝:1];希望能好吃。

依次为：**桌号;订单内容;订单总价格;订单备注**。可见，不同数据段通过英文标点`;`分隔。因此可用*QString*类中自带的`QString::section()`进行分割。分割好的消息填入名为*_table_Orders*的*QTableWidget*中。并同时更新订单统计信息:`_OrdersCount++;`，`_OrdersNoCount++;`。*_OrdersCount*存储**总订单数**，*_OrdersNoCount*存储未处理订单数。

订单号共有19位数字，格式如下：

> 2020052918563100001
> 2020052918563199999

固定前缀依次为：**年月日小时分钟秒**（共14位）

后缀为：**00000-99999**（共5位）

点击*btnHandle*和*btnReHandle*按钮将处理或者重新处理订单。处理和重新处理订单时应判断一下*_table_Orders*是否已选中一行。

订单受到处理后，启动一个*QTimer*:`clearTimer->start(_clearShot*1000);`，并将QTimer设置为单次执行`clearTimer->setSingleShot(true); `。在规定好的*_clearShot*时间后将订单从*_table_Orders*中删除并存入本地Sqlite数据库即历史订单数据库中。删除功能目前采用遍历*_table_Orders*的方法来删除指定订单。

再讲一下界面控件相关细节问题。

1. 订单查看通过重写的*QDialog*类的继承类*DialogOrdersViewer*来查看。通过参数传递，支持查看订单状态，订单单号，订单桌号，订单内容和订单备注。

2. *_table_Orders*进行特殊配置。

   ```cpp
   //让“订单内容”和订单备注随拉伸变化
   _table_Orders->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Stretch);
   _table_Orders->horizontalHeader()->setSectionResizeMode(5,QHeaderView::Stretch);
   
    //设置table属性
   _table_Orders->setFocusPolicy(Qt::NoFocus);  //失去焦点，让_table_Orders在未选中前不能删除 
   _table_Orders->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行行为，以行为单位    
   _table_Orders->setSelectionMode(QAbstractItemView::SingleSelection); //设置选择模式，选择单行 
   _table_Orders->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置为禁止编辑
   ```

   

3. 新订单声音提醒。

   ```cpp
   //Sound
   sound = new QSound(":/Res/clock.wav");
   
   //接收到新订单时：
   sound->play();
   ```

   

4. 状态栏信息更新问题。通过以下方式初始化订单栏。

   ``` cpp
   // Status Bar
   labelOrdersNoCount = new QLabel(tr("未处理订单数：%1").arg(QString::number(_OrdersNoCount)),this);
   labelOrdersCount = new QLabel(tr("总订单数：%1").arg(QString::number(_OrdersCount)),this);
   labelTime = new QLabel(getFormatTimeStamp("yyyy-MM-dd hh:mm:ss"),this);
   
   ui->statusbar->addPermanentWidget(labelOrdersNoCount);
   ui->statusbar->addPermanentWidget(labelOrdersCount);
   ui->statusbar->addPermanentWidget(labelTime);
   //启动计时器
   timer = new QTimer(this);
   connect(timer,&QTimer::timeout,[=](){
       labelTime->setText(getFormatTimeStamp("yyyy-MM-dd hh:mm:ss"));
   });
   timer->start(1000);
   
   //接收到新订单时：
   //更新订单统计信息
   _OrdersCount++; //总订单数加1
   _OrdersNoCount++; //未处理订单数加1
   
   //更新状态栏
   ui->statusbar->showMessage("有新的订单！",2000);
   labelOrdersNoCount->setText(tr("未处理订单数：%1").arg(QString::number(_OrdersNoCount)));
   labelOrdersCount->setText(tr("总订单数：%1").arg(QString::number(_OrdersCount)));
   ```

   

   

#### 历史订单查看

通过在*tab_Orders*界面点击*btnViewer*进入*DialogHistoryViewer*历史订单查看界面。

历史订单中具体订单查看仍然使用*DialogOrdersViewer*来查看。

历史订单查看支持全部查看和按时间筛选部分订单查看。通过对订单号前缀（时间）进行筛选。

界面细节逻辑：

1. 单选按钮组将会影响时间编辑框的显示。“全部显示”选中，无时间编辑框显示，“显示之前”选中，仅显示一个时间编辑框，“显示之间”，显示两个时间编辑框。
2. 目前性能较低，在每次点选不同单选按钮之后，将会重新遍历显示全部历史订单。





#### 查看修改菜单信息

使用*QSqlDatabase*,*QSqlTableModel*,*QSqlTableView*,*QSqlQuery*等Qt封装库实现相关功能。主要功能如下：

##### 核心功能

###### 查

遍历整个数据库，*QSqlTableModel*与*QSqlTableView*结合，将菜单数据库列出。

###### 增

单击增加菜品按钮，加载*DialogAddRecord*界面类进行菜品添加。将图片文件转存到本地缓存，上传到Http服务器。

###### 删

单击删除菜品按钮，判断是否选中行后，使用*QSqlTableModel*的`removeRow()`方法删除数据库记录。删除本地图片文件缓存和Http服务器对应文件。

###### 改

单击修改菜品按钮，判断是否选中行后，加载*DialogEditRecord*界面类进行菜品修改。若图片更改，删除旧的本地图片缓存和Http服务器旧的图片文件，将新的图片转存到本地缓存并上传到Http服务器。

##### 非核心的关键功能

###### 服务端本地图片缓存

使用*QDir*和*QFile*封装类，将增、改的图片拷贝到程序运行目录下的`/Pic`图片缓存目录下。文件名与菜品名同名。

###### 服务端远程图片储存

使用*HttpFileUpload*类和*HttpFileUpdate*类进行图片文件上传和删除更新。

*upload_file.php*文件内容如下：

```php
<?php
// 允许上传的图片后缀
$allowedExts = array("gif", "jpeg", "jpg", "png");
$temp = explode(".", $_FILES["file"]["name"]);
echo $_FILES["file"]["size"];
$extension = end($temp);     // 获取文件后缀名
if ((($_FILES["file"]["type"] == "image/gif")
|| ($_FILES["file"]["type"] == "image/jpeg")
|| ($_FILES["file"]["type"] == "image/jpg")
|| ($_FILES["file"]["type"] == "image/pjpeg")
|| ($_FILES["file"]["type"] == "image/x-png")
|| ($_FILES["file"]["type"] == "image/png"))
&& ($_FILES["file"]["size"] < 5242880)   // 小于 5mb
&& in_array($extension, $allowedExts))
{
    if ($_FILES["file"]["error"] > 0)
    {
        echo "错误：: " . $_FILES["file"]["error"] . "<br>";
    }
    else
    {
        echo "上传文件名: " . $_FILES["file"]["name"] . "<br>";
        echo "文件类型: " . $_FILES["file"]["type"] . "<br>";
        echo "文件大小: " . ($_FILES["file"]["size"] / 1024) . " kB<br>";
        echo "文件临时存储的位置: " . $_FILES["file"]["tmp_name"] . "<br>";
        
        // 判断当前目录下的 upload 目录是否存在该文件
        // 如果没有 upload 目录，你需要创建它，upload 目录权限为 777
        if (file_exists("upload/" . $_FILES["file"]["name"]))
        {
            echo $_FILES["file"]["name"] . " 文件已经存在。 ";
        }
        else
        {
            // 如果 upload 目录不存在该文件则将文件上传到 upload 目录下
            move_uploaded_file($_FILES["file"]["tmp_name"], "upload/" . $_FILES["file"]["name"]);
            echo "文件存储在: " . "upload/" . $_FILES["file"]["name"];
        }
    }
}
else
{
    echo "非法的文件格式";
}
?>
```

*update.php*文件内容如下：

```php
<?php
    $file_name = $_POST['file_name'];
    $path = 'upload';
    $result = 0;

    if(file_exists($path.'/'.$file_name))
    {
        $result = unlink($path.'/'.$file_name);
    }
    else
    {
        $result = 1;
    }

    return $result;
?>
```



  

###### 向客户端发送更新菜单消息

当服务端修改菜单数据库后，发送`sendMenuUpdateSignal()`消息，调用匹配好的槽函数`slotSendMenuUpdateMessage()`向客户端发送消息。应使用循环`for(int i=0; i<_tcpSocket.size(); i++)`向全部在线客户端发送消息：

```cpp
QTcpSocket *currentSocket;

for(int i=0; i<_tcpSocket.size(); i++)
{
    currentSocket = _tcpSocket.at(i);
    currentSocket->write(QString("[Menu Updated]").toUtf8());
}
```





#### 修改服务器配置

在*tab_Settings*中对服务器配置进行修改。主要有以下配置的修改：

* MySql数据库配置
  * MySql服务器地址
  * MySql服务器端口
  * MySql数据库名称
  * MySql数据库用户名
  * MySql数据库密码
* 图片HTTP服务器配置
  
  * HTTP服务端地址
* TCP设置
  * TCP服务端IP
  * TCP服务端端口

   其中，MySql与HTTP配置储存在目录下的*config.json*文件中。json文件的读写吗，使用自己封装的*WriteJson*类和*ReadJson*类实现。为了保证安全性，json文件中的部分内容使用自己封装的**异或加密解密**（不能保证完全安全） *EncryptDecrypt*类加密解密。

*config.json*文件内容如下：

```json
{
    "clearShot": 30,
    "dbHost": "dbHost",
    "dbName": "dbName",
    "dbPasswd": "dbPasswd",
    "dbPort": 3306,
    "dbUser": "dbUser",
    "picHost": "picHost",
    "tcpHost": "127.0.0.1",
    "tcpPort": 8081
}
```



   

















### 客户端

客户端主要实现四个功能。菜单展示，购物车功能，发送订单信息和修改客户端配置。

客户端主要涉及Tcp操作与MySql以及Http操作。客户端使用Tcp实时与服务端进行信息交换；使用远程MySql读取菜单数据；客户端使用http请求模拟来下载远程http服务器中的图片。



#### 菜单展示

客户端主界面使用`QListWidget`显示菜品信息。启动时将首先调用`ClientMainWindow::loadMenu()`,通过如下方式遍历远程MySQL数据库中的*MenuType*表与*menu*表，将菜品信息储存。

```cpp
void ClientMainWindow::loadMenu()  //加载菜品
{
    ...
}
```

首先应清除之前储存的信息并将购物车清空。

```cpp
ui->statusbar->showMessage("正在更新菜单信息，请稍等..."); //更新一下状态栏消息

/* 清除之前的数据 */
if(_menuList->count())  //菜单列表
{
    _menuList->clear();
}

if(!_menuTypeList.isEmpty())  //菜品种类列表
{
    _menuTypeList.clear();
}

if(!_menuNameList.isEmpty())  //菜品名列表
{
     _menuNameList.clear();
}

if(!_menuTypeNumList.isEmpty())  //菜品种类名列表
{
    _menuTypeNumList.clear();
}

if(!_menuFileNameList.isEmpty())  //菜品图片文件名列表
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
```

接着遍历一下*menuType*表，记录一下菜品分类。注意`select`完表后，应先执行一下`next()`让他指向第一条记录。

```cpp
_alreadyDownloadMenuCount=0;
 _menuCount=0;

//先遍历menuType表，记录菜品分类
QSqlQuery query(db);
qDebug()<<query.exec("SELECT * FROM menuType WHERE 1");
int menuTypeCount = query.size();  //菜品种类总共数量

qDebug()<<"更新："<<menuTypeCount;
query.next();  //必须执行一下.next()让他指向第一条记录，否则记录会指向第一条记录之前的记录

for(int i=1;i<=menuTypeCount;i++)
{
    _menuCount+=query.value(2).toInt(); //_menuCount记录着一共多少菜品（是总菜品数！而不是总菜品种类数！）

    query.next();
}
```

记录完菜品分类后，就要以**菜品种类**为单位进行循环，遍历*menu*表，将每一个菜品记录，并同时将菜品图片下载到缓存文件夹中。这里注意应该先将*query*查询类使用`first()`重新指向第一条记录。

```cpp

    query.first();  //重新指向第一条记录

    for(int i=1;i<=menuTypeCount;i++)
    {
        //添加一种菜品种类
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
            //添加一个菜品
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

                emit signalAddAlreadyDownloadMenuCount();  //下载完一张图片，就发送一个下载完成消息

            });

            dishQuery.next();
        }


        query.next();
    }

```



这里应该注意一下菜品信息显示的时机，应该待全部菜品图片下载结束后才将菜品显示到*QListWidget*中。因而每当下载完图片后，都要发送一次图片下载完成消息`void signalAddAlreadyDownloadMenuCount()`。

在`void slotAddAlreadyDownloadMenuCout()`槽函数中，将已经下载的图片数`_alreadyDownloadMenuCount`累加，当`_alreadyDownloadMenuCount == _menuCount`即图片全部下载完成时，执行`insertItems()`，将菜品信息插入到*QListWidget*中显示出来。

```cpp
void ClientMainWindow::slotAddAlreadyDownloadMenuCount()
{
    _alreadyDownloadMenuCount++;  //已经下载的图片数+1

    qDebug()<<"alreadyDownloadMenuTypeCount:"<<_alreadyDownloadMenuCount;


    if(_alreadyDownloadMenuCount==_menuCount)  //如果图片全部下载完成
    {
        insertItems();  //菜品图片全部下载完成后，准备开始添加菜品信息到listwidget中
    }
}
```

`insertItems()`应下插入菜种类头，再插入菜种类下的头。

```cpp
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

```



当单击某项菜品后，应将菜品详细信息展示在*GroupBox*中。

```cpp
void ClientMainWindow::slotItemClicked(QListWidgetItem *item)  //选择一个菜品，展示菜品详细信息
{
    qDebug()<<"item clicked";
    QString itemStr = item->text();
    //分离菜品信息
    QString dishName = itemStr.mid(0,itemStr.indexOf("\t"));
    QString dishPrice = itemStr.mid(itemStr.indexOf("\t")+1,itemStr.indexOf("\n")-itemStr.indexOf("\t")-1);
    QString dishInfo = itemStr.mid(itemStr.lastIndexOf("\n")+1,-1);
    QString dishPhotoFileName;

    //图片信息分离
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

    //展示菜品信息
    lb_pic->setPixmap(QPixmap(dishPhotoFileName));
    lb_dishNameContent->setText(dishName);
    lb_dishPriceContent->setText(dishPrice);
    tb_dishInfo->setText(dishInfo);
}
```





#### 购物车功能

购物车功能首先在菜品展示页中放置一个重写的*MyButton*类动画图片按钮。当菜品添加到购物车时，播放一下购物车按钮的动画。

单击购物车按钮，也播放一个动画然后进入购物车查看界面。

```cpp
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
```

购物车查看界面由对话框类*DialogCartView*实现。购物车查看界面要实现对购物车内菜品数量修改的功能和购物车清空、结算功能。

##### 购物车内菜品数量修改

当双击编辑购物车数量后，调用`slotCartChanged(QTableWidgetItem *item)`槽函数进行处理。重新计算菜品总价和菜品总数并更新购物车界面显示信息，也要向*ClientMainWindow*发送购物车列表改变的消息。

```cpp
void DialogCartView::slotCartChanged(QTableWidgetItem *item)
{

    qDebug()<<"row:"<<item->row()<<" "<<"column:"<<item->column();

    QTableWidgetItem *currentItemPrice = new QTableWidgetItem;
    currentItemPrice->setText(QString::number(_cartList.at(item->row()).getSum())); //重新计算当前菜品总价


    const_cast<CartItem&>(_cartList.at(item->row())).setItemNums(item->text().toInt());

    //_cartPrice和_cartNum清零
    _cartPrice = 0;
    _cartNum = 0;

    //计算当前购物车总价
    for(int i=0;i<_cartList.size();i++)
    {
        _cartPrice+=_cartList.at(i).getSum();
    }
    //计算当前购物车总数
    for(int i=0;i<_cartList.size();i++)
    {
        _cartNum+=_cartList.at(i).getNum();
    }

    //更新购物车信息
    lb_cartNumContent->setText(QString::number(_cartNum));
    lb_cartPriceContent->setText(QString::number(_cartPrice));

    emit signalCartChanged(_cartList);  //发送购物车改变消息
}
```

*ClientMainWindow*中关联的槽函数`slotCartChanged(QList<CartItem>changedCart)`

```cpp
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
```





##### 购物车清空功能

单击清空购物车按钮，调用对应槽函数清空购物车信息，发送购物车清空消息。

```cpp
void DialogCartView::btnClearClicked()
{
    int ret = QMessageBox::question(this,"请求确认","您确认要清空购物车吗？");

    if(ret == QMessageBox::Yes)
    {
        _cartList.clear(); //购物车列表清空

        for(int i=_cartTable->rowCount()-1;i>=0;i--)  //展示的tablewidget倒序清空
        {
            _cartTable->removeRow(i);
        }

        //更新购物车信息
        lb_cartNumContent->setText("0");
        lb_cartPriceContent->setText("0");

        emit signalCartCleaned();  //发送购物车清空信息
    }
}
```

*ClientMainWindow*中关联的槽函数`slotCartCleaned()`

```cpp
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
```





##### 购物车结算功能

单击结算按钮，调用对应槽函数处理，发送购物车结算消息。

```cpp
void DialogCartView::btnCheckOutBtnClicked()
{
    if(_cartTable->rowCount()==0)
    {
        QMessageBox::critical(this,"错误","购物车为空！");
        return;
    }
    emit signalCartCheckOut();  //发送购物车结算消息
    this->close();
}
```

*ClientMainWindow*中关联的槽函数`slotCartCheckOut()`将初始化对话框类*DialogCheckout*，进入订单结算界面。

```cpp
void ClientMainWindow::slotCartCheckOut()
{
    DialogCheckOut *dlg = new DialogCheckOut(cartLists,_cartNumCount,_cartPriceCount);
    dlg->show();

    void (DialogCheckOut::*pSignalReadyCheckOut)(QString) = &DialogCheckOut::signalReadyCheckOut;
    void (ClientMainWindow::*pSlotReadyCheckOut)(QString) = &ClientMainWindow::slotReadyCheckOut;
    connect(dlg,pSignalReadyCheckOut,this,pSlotReadyCheckOut);
}
```

订单结算界面将显示订单信息以便最后确认，并询问用户输入订单备注。

用户点击结算按钮后，调用对应槽函数向*ClientMainWindow*发送`void signalReadyCheckOut(QString note)`消息。

```cpp
connect(btn_checkout,&QPushButton::clicked,[=](){
    int ret = QMessageBox::question(this,"请求确认","您确认要提交此订单吗？");
    if(ret == QMessageBox::Yes)
    {
        emit signalReadyCheckOut(_textNote->toPlainText()); //发送结算消息
        this->close();
    }
});
```

*ClientMainWindow*中对应处理的槽函数`slotReadyCheckOut(QString note)`将完成结算并向服务端发送订单socket信息。

```cpp
void ClientMainWindow::slotReadyCheckOut(QString note)  //结帐，发送socket信息
{
    qDebug()<<"slotReadyCheckOut";

/*
	A03;125;[宫保鸡丁:1],[老八小汉堡:2],[扬州炒饭:2],[鱼香肉丝:1];希望能好吃。
*/
    QStringList dataList; //用一个QStringList来存取要发送的订单socket信息
    dataList<<_tableNum<<";";
    dataList<<QString::number(_cartPriceCount)<<";";

    QString cartContent;  //讲购物车菜品信息格式化合并并存入到datalist
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

    QString data;  //讲datalist转为整个的QString
    foreach(QString s, dataList)
    {
        data+=s;
    }
    qDebug()<<data;

    bool ret = client->sendData(data.toUtf8());  //发送socket信息
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
```





#### 订单信息发送

客户端启动后，将首先调用`TcpClient`类，使用`TcpClient::establishConnect(QString host, int port)`建立与服务端的socket连接。

当用户下单时，将调用槽函数`slotReadyCheckOut(QString note)`向服务端发送订单socket信息，详细内容见上。





#### 修改客户端配置

用户在界面菜单中选择修改客户端配置时进入客户端配置修改界面，与服务端配置修改功能相似。





























____





## TODO LISTS

### 服务端

1. ~~任务配置tab加入Http服务器信息修改，读取、写入json功能也做相应增改。~~
2. ~~删除订单时删除本地存储的图片，以及远程存取的图片。~~
3. ~~菜单信息更新（包括增加、删除、修改）后，向客户端发送消息，要求客户端更新菜单数据。~~
4. ~~新建一个menuType表，记录菜品种类和对应种类的菜品数。修改、增加菜单信息时更新该menuType表。~~
5. ~~完善历史订单查看功能。（目前问题：第一次搜寻结果为空白时，初始化后才能进行下一次搜寻，否则结果为空）~~
6. 
7. 长远目标：菜品销售分析。
8. 优化！优化！优化！



### 客户端

1. ~~菜单列表显示~~
2. ~~客户端设置~~
3. ~~菜品详细信息查看~~
4. ~~购物车功能实现~~
5. ~~接收服务端菜单更新信息，实时更新菜单。~~
6. ~~状态栏实时菜单更新消息，购物车数目、总价和实时时间更新。~~
7. ~~购物车列表添加代理控件~~
8. 
9. 优化！优化！优化！



















​                                          																														*作者：skykey*

------

  

  

  