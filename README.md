# Ordering-System

重构进行ing...

预测是大版本颠覆= =

## 项目简介

一个基于Qt5实现的自主订餐系统。大量使用了TCP，Sqlite相关技术，~~支持多平台操作~~（理论上能跑Qt的操作环境就支持）。

### 功能设计

* 服务端实现4个主要功能：

    * **实时**接收客户端订单，并能对订单进行查看、处理和重新处理已经处理的订单。已经处理的订单在**规定时间**后会归档。使用*QTcpServer*和*QTcpSocket*实现服务端与客户端通信。
    * 查看历史订单。历史订单储存在**本地**Sqlite数据库中。使用*QSqlDataBase*等数据库操作封装库实现。
    * 查看、修改菜单信息，支持对菜单进行增、删、查、改操作。为支持服务端、多客户端同时查看菜单信息，将菜单信息储存在**远程**MySql数据库中。使用*QSqlDataBase*等数据库操作封装库以及*
      QNetworkAccessManager*等封装库实现。
    * 对服务端配置进行修改，如远程Mysql服务端信息，本地Tcp服务端信息。涉及Json文件的读写。使用到了*QJson*等Json文件操作相关封装库。

* 客户端实现4个主要功能

    * 显示菜单，支持添加购物车。
    * 对购物车进行增删
    * 提交订单至服务端
    * 修改客户端相关配置信息，如服务端Tcp服务信息。

### 架构设计

菜单信息、订单历史记录储存在服务端本地的SQLite数据库中。

服务端集成一个轻量WEB服务器，为客户端提供图片下载功能。

服务端编辑更新菜单后，向所有客户端发送菜单更新消息，提醒客户端更新本地菜单信息，菜单更新消息需具体到菜品， 并表明是否需要重新下载菜品图片。 某个客户端（启动时必须）可通过向服务端发送菜单请求消息，获取服务端返回的完整菜单。

服务端、客户端通过两条TCP Socket进行通信：

* 客户端有三种消息：
    * 请求菜单 `0`
    * 新订单 `1`
    * 心跳包 `2` （2号信道）
* 服务端消息：
    * 菜单请求返回 `0`
    * 菜单更新 `1`
        * 菜品:
            * `0` 添加菜品
            * `1` 更新菜品信息
            * `2` 删除菜品
        * 菜品种类：
            * `0` 添加菜品种类
            * `1` 更新菜品种类信息
            * `2` 删除菜品
    * 客户端心跳包回应 `2` （2号信道）

客户端JSON格式：

* 请求菜单：

  ```json
  {
    "code": 0
  }
  ```

* 新订单：
  ```json
  {
    "code": 1,
    "Table":"A03", 
    "OrderNum": "A032021109150031",
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
  ```

* 心跳包：
  ```json
  {
    "code": 2,
    "Msg": "Heart"
  }
  ```

服务端JSON格式：

* 菜单请求返回：
  ```json
  {
    "code": 0,
    "menu": [
      {
        "Name": "青椒肉丝",
        "Type": "小炒",
        "Info": "家常小炒，富含维生素C",
        "Price": 12.0,
        "Photo": "QingJiaoRouSi.png"
      },
      {
        "Name": "五花肉鸡公煲",
        "Type": "鸡公煲",
        "Info": "五花肉鸡公煲，分量超足！",
        "Price": 25.0,
        "Photo": "WuHuaRouJiGongBao.png"
      },
      {
        "Name": "酸辣土豆丝",
        "Type": "家常小炒",
        "Info": "酸酸辣辣，鲁菜经典",
        "Price": 6.0,
        "Photo": "SuanLaTuDouSi.png"
      },
      {
        "Name": "原味鸡公煲",
        "Type": "鸡公煲",
        "Info": "原味鸡公煲，原汁原味，分量超足！",
        "Price": 20.0,
        "Photo": "JiGongBao.png"
      }
    ],
    "menuType":[
      {
        "Name": "小炒",
        "Num": 2
      },
      {
        "Name": "鸡公煲",
        "Num": 2
      },
      {
        "Name": "凉菜",
        "Num": 0
      },
      {
        "Name": "饺子馄饨",
        "Num": 0
      },
      {
        "Name": "面",
        "Num": 0
      } 
    ]
  }
  ```
* 菜单更新
  ```json
  {
    "code": 1,
    "menu": [
      {
        "Opt": 0,
        "Name": "白菜猪肉馅蘸饺",
        "Type": "饺子馄饨",
        "Info": "经典家常蘸饺",
        "Price": 8.0,
        "Photo": "BaiCaiZhuRouXianZhanJiao.png"
      },
      {
        "Opt": 0,
        "Name": "莲藕猪肉馅蘸饺",
        "Type": "饺子馄饨",
        "Info": "莲藕猪肉馅蘸饺",
        "Price": 8.5,
        "Photo": "LianOuZhuRouXianZhanJiao.png"
      },
      {
        "Opt": 1,
        "Name": "五花肉鸡公煲",
        "Type": "鸡公煲",
        "Info": "五花肉鸡公煲，现特价销售！",
        "Price": 21.0,
        "Photo": "WuHuaRouJiGongBao.png",
        "PhotoUpdated": false
      },
      {
        "Opt": 0,
        "Name": "老醋花生",
        "Type": "凉菜",
        "Info": "撸串必点",
        "Price": 5.0,
        "Photo": "LaoCuHuaSheng.png"
      },
      {
        "Opt": 1,
        "Name": "青椒肉丝",
        "Type": "小炒",
        "Info": "家常小炒，富含维生素C",
        "Price": 12.0,
        "Photo": "QingJiaoRouSi.png",
        "PhotoUpdated": true
      },
      {
        "Opt": 2,
        "Name": "酸辣土豆丝"
      }
    ],
    "menuType": [
      {
        "Opt": 0,
        "Name": "干锅"
      },
      {
        "Opt": 1,
        "Name": "饺子馄饨",
        "Num": 2
      },
      {
        "Opt": 1,
        "Name": "小炒",
        "Num": 1
      },
      {
        "Opt": 1,
        "Name": "凉菜",
        "Num": 1
      },
      {
        "Opt": 2,
        "Name": "面"
      }
    ]
  }
  ```
* 心跳包回复
  ```json
  {
    "code": 2,
    "Msg": "OK"
  }
  ```
  ```json
  {
    "code": 2,
    "Msg": "HEART_BACK"
  }
  ```

若服务端查询到某客户端超时，则从客户端列表中删除该客户端。若客户端检测到与服务器链接断开，则报错。

（菜单）数据库操作封装在现成的类内，服务端使用封装好的接口操作服务器。客户端不需要接触数据库操作。

服务器端维护一个简易HTTP服务器，提供菜单照片下载。

订单单号：桌号+当前时间（年月日小时分钟秒）`A032021109150031`

### 项目截图：

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

服务端程序启动后，会创建名为*_tcpServer*的Tcp服务端，并使用`_tcpServer->listen(QHostAddress::Any, _tcpPort)`启动监听（*_tcpPort*
来自于配置文件）。并通过`connect(_tcpServer,&QTcpServer::newConnection,this,&ServerMainWindow::slotNewConnection)`
绑定信号与槽，当有客户端连接时，调用*slotNewConnection*()方法。

*slotNewConnection()*将会通过`while(_tcpServer->hasPendingConnections())`判断是否有未处理的连接，将新的socket连接存入*QList<QTcpSocket \* >*
类型的*_tcpSocket*中。并使用`connect(currentSocket,&QTcpSocket::readyRead,this,&ServerMainWindow::slotReadyRead)`为新的scoket提供槽函数*
slotReadyRead()*，来接受数据。

*slotReadyRead()*循环`readAll()`接收数据。数据格式如下：

> A03;125;[宫保鸡丁:1],[老八小汉堡:2],[扬州炒饭:2],[鱼香肉丝:1];希望能好吃。

依次为：**桌号;订单内容;订单总价格;订单备注**。可见，不同数据段通过英文标点`;`分隔。因此可用*QString*类中自带的`QString::section()`进行分割。分割好的消息填入名为*_table_Orders*的*
QTableWidget*中。并同时更新订单统计信息:`_OrdersCount++;`，`_OrdersNoCount++;`。*_OrdersCount*存储**总订单数**，*_OrdersNoCount*存储未处理订单数。

订单号共有19位数字，格式如下：

> 2020052918563100001
> 2020052918563199999

固定前缀依次为：**年月日小时分钟秒**（共14位）

后缀为：**00000-99999**（共5位）

点击*btnHandle*和*btnReHandle*按钮将处理或者重新处理订单。处理和重新处理订单时应判断一下*_table_Orders*是否已选中一行。

订单受到处理后，启动一个*QTimer*:`clearTimer->start(_clearShot*1000);`，并将QTimer设置为单次执行`clearTimer->setSingleShot(true); `。在规定好的*_
clearShot*时间后将订单从*_table_Orders*中删除并存入本地Sqlite数据库即历史订单数据库中。删除功能目前采用遍历*_table_Orders*的方法来删除指定订单。

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

当服务端修改菜单数据库后，发送`sendMenuUpdateSignal()`消息，调用匹配好的槽函数`slotSendMenuUpdateMessage()`
向客户端发送消息。应使用循环`for(int i=0; i<_tcpSocket.size(); i++)`向全部在线客户端发送消息：

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

  其中，MySql与HTTP配置储存在目录下的*config.json*文件中。json文件的读写吗，使用自己封装的*WriteJson*类和*ReadJson*类实现。为了保证安全性，json文件中的部分内容使用自己封装的**
  异或加密解密**（不能保证完全安全） *EncryptDecrypt*类加密解密。

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

客户端主界面使用`QListWidget`显示菜品信息。启动时将首先调用`ClientMainWindow::loadMenu()`, 向服务端发送菜单请求信息，并储存服务器返回的菜单。

首先应清除之前储存的信息。

```cpp
if (!_dishes.isEmpty()) // 菜品列表
	_dishes.clear();
if (_menuTypeList.isEmpty())  // 菜品种类列表
	_dishes.clear();
if (!_menuTypeNumHash.isEmpty())  // 菜品种类-数量键值对
	_menuTypeNumHash.clear();
if (!_dishNameAndFileNameHash.isEmpty()) // 菜品名-菜品文件名键值对
	_dishNameAndFileNameHash.clear();
```

接着将返回的json解析并储存。

首先将菜品的文件信息插入*QListWidget*中，后利用线程池多线程下载菜品图片，并将其加载到对应菜品栏中。

当单击某项菜品后，应将菜品详细信息展示在*GroupBox*中。通过检测该栏的icon是否为空检测是否为具体菜品。

#### 购物车功能

购物车功能首先在菜品展示页中放置一个重写的*MyButton*类动画图片按钮。当菜品添加到购物车时，播放一下购物车按钮的动画。

单击购物车按钮，也播放一个动画然后进入购物车查看界面。

购物车查看界面由对话框类*DialogCartView*实现。购物车查看界面要实现对购物车内菜品数量修改的功能和购物车清空、结算功能。

##### 购物车内菜品数量修改

当双击编辑购物车数量后，调用`slotCartChanged(QTableWidgetItem *item)`槽函数进行处理。重新计算菜品总价和菜品总数并更新购物车界面显示信息，也要向*ClientMainWindow*
发送购物车列表改变的消息。

*ClientMainWindow*中关联的槽函数`slotCartChanged(QList<CartItem>changedCart)`

购物车内容修改后要同步修改`_cartSet`与`_cartList`。

##### 购物车清空功能

单击清空购物车按钮，调用对应槽函数清空购物车信息，发送购物车清空消息。

*ClientMainWindow*中关联的槽函数`slotCartCleaned()`

购物车清空后要同步修改`_cartSet`与`_cartList`。

##### 购物车结算功能

单击结算按钮，调用对应槽函数处理，发送购物车结算消息。

*ClientMainWindow*中关联的槽函数`slotCartCheckOut()`将初始化对话框类*DialogCheckout*，进入订单结算界面。

订单结算界面将显示订单信息以便最后确认，并询问用户输入订单备注。

用户点击结算按钮后，调用对应槽函数向*ClientMainWindow*发送`void signalReadyCheckOut(QString note)`消息。

*ClientMainWindow*中对应处理的槽函数`slotReadyCheckOut(const QString &note)`将完成结算并向服务端发送订单socket信息。

#### 订单信息发送

客户端启动后，将首先调用`TcpClient`类，使用`TcpClient::establishConnect(QString host, int port)`建立与服务端的socket连接。

当用户下单时，将调用槽函数`slotReadyCheckOut(const QString &note)`向服务端发送订单socket信息，详细内容见源码。

#### 修改客户端配置

用户在界面菜单中选择修改客户端配置时进入客户端配置修改界面，与服务端配置修改功能相似。

____

作者：Skykey

------

  

  

  