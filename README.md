# Diana Just Eat, An Ordering-System

关注嘉然，顿顿解馋。

## 项目简介

一个基于Qt5实现的自主订餐系统。大量使用了TCP，Sqlite相关技术，~~支持多平台操作~~（理论上能跑Qt的操作环境就支持）。

### 功能设计

* 服务端实现4个主要功能：

    * **实时**接收客户端订单，并能对订单进行查看、处理和重新处理已经处理的订单。已经处理的订单在**规定时间**后会归档。使用*QTcpServer*和*QTcpSocket*实现服务端与客户端通信。
    * 查看历史订单。历史订单储存在**本地**Sqlite数据库中。使用*QSqlDataBase*等数据库操作封装库实现。
    * 查看、修改菜单信息，支持对菜单进行增、删、查、改操作。为支持服务端、多客户端同时查看菜单信息，将菜单信息储存在**远程**MySql数据库中。使用*QSqlDataBase*等数据库操作封装库以及*
      QNetworkAccessManager*等封装库实现。
    * 对服务端配置进行修改，如远程Mysql服务端信息，本地Tcp服务端信息。涉及Json文件的读写。

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

当客户端发现心跳包回复超时，则采取断连处理。若客户端检测到与服务器链接断开，则报错。

（菜单）数据库操作封装在现成的类内，服务端使用封装好的接口操作服务器。客户端不需要接触数据库操作。

服务器端维护一个简易HTTP服务器，提供菜单照片下载。

订单单号：桌号+当前时间（年月日小时分钟秒）`A032021109150031`

### 项目截图：

服务端——订单界面：

<img src="/Screenshots/server1.jpg" title="服务端——订单界面" style="zoom:70%;" />



服务端——订单查看：

<img src="/Screenshots/server2.jpg" title="服务端——查看订单" style="zoom:70%;" />



服务端——历史订单查看：

<img src="/Screenshots/server3.jpg" title="服务端——历史订单查看" style="zoom:70%;" />



服务端——历史订单查看（显示之前）：

<img src="/Screenshots/server4.jpg" title="服务端——历史订单查看" style="zoom:70%;" />



服务端——历史订单查看（显示之间）：

<img src="/Screenshots/server5.jpg" title="服务端——历史订单查看" style="zoom:70%;" />



服务端——历史订单图表（年销售量）：

<img src="/Screenshots/server6.jpg" title="服务端——历史订单图表（年销售量）" style="zoom:70%;" />



服务端——历史订单图表（月销售量）：

<img src="/Screenshots/server7.jpg" title="服务端——历史订单图表（月销售量）" style="zoom:70%;" />



服务端——历史订单图表（日销售量）：

<img src="/Screenshots/server8.jpg" title="服务端——历史订单图表（日销售量）" style="zoom:70%;" />



服务端——菜单界面：

<img src="/Screenshots/server9.jpg" title="服务端——菜单界面" style="zoom:70%;" />



服务端——编辑菜品：

<img src="/Screenshots/server10.jpg" title="服务端——编辑菜品" style="zoom:70%;" />



服务端——添加菜品：

<img src="/Screenshots/server11.jpg" title="服务端——添加菜品" style="zoom:70%;" />



服务端——编辑菜品种类：

<img src="/Screenshots/server12.jpg" title="服务端——编辑菜品种类"  />



服务端——设置：

<img src="/Screenshots/server13.jpg" title="服务端——设置" style="zoom:70%;" />



客户端——主界面：

<img src="/Screenshots/client1.jpg" title="客户端——主界面" style="zoom:70%;" />



客户端——购物车：

<img src="/Screenshots/client2.jpg" title="客户端——购物车" />



客户端——订单结算：

<img src="/Screenshots/client3.jpg" title="客户端——订单结算"/>



客户端——设置：

<img src="/Screenshots/client4.jpg" title="客户端——设置"/>


------

## 具体功能实现

### 服务端

服务端主要由三个Tab组成，分别对应着实时订单接收与处理、查看历史订单，查看修改菜单信息，修改服务器配置四个功能。

服务端主要涉及Tcp操作、Sqlite以及Http操作。服务端使用Tcp实时与客户端进行信息交换；使用本地Sqlite存取菜单数据； 使用本地Sqlite存取历史订单。服务端将图片存在本地Http服务器中，方便客户端与服务端同时访问。

菜单数据结构如下：

```sqlite
.DataBase: Ordering-Sytem
.TABLES: menu, menuType, config
/menu       --菜品数据 
  |-ID-|-Name-|-Type-|-Info-|-Price-|-Photo-|
(ID INT PRIMARY KEY AUTOINCREMENT, Name TEXT, Type TEXT, Info TEXT, Price DOUBLE, Photo Text)
/menuType   --菜品种类数据
  |-ID-|-TypeName-|-Num-|
(ID INT PRIMARY KEY AUTOINCREMENT, TypeName Text, Num Text)
```

订单数据结构如下：

```sqlite
.DataBase: orders.db
.TABLES: orders
/orders   --历史订单
  |-ID-|-OrderNum-|-TableNum-|-OrderPrice-|-OrderContent-|-OrderNote-|
(ID INTEGER PRIMARY KEY AUTOINCREMENT, OrderNum TEXT, OrderTableNum TEXT, OrderPrice DOUBLE, OrderContent TEXT, OrderNote TEXT)
```

利用第三方库实现一个简易HTTP WEB服务器，将本地`Pic`目录中的文件夹设为Web服务器根目录。

#### 实时订单接收与处理

服务端启动后，调用`startTcpServer()`方法建立起TCP服务端，共有两个TCP Server，一个是订单菜单信道，一个是状态信道。

订单受到处理后，启动一个*QTimer*:`clearTimer->start(_clearShot*1000);`，并将QTimer设置为单次执行`clearTimer->setSingleShot(true); `。在规定好的*_
clearShot*时间后将订单从*_table_Orders*中删除并存入本地Sqlite数据库即历史订单数据库中。删除功能目前采用遍历*_table_Orders*的方法来删除指定订单。

再讲一下界面控件相关细节问题。

1. 订单查看通过重写的*QDialog*类的继承类*DialogOrdersViewer*来查看。通过参数传递，支持查看订单状态，订单单号，订单桌号，订单内容和订单备注。
2. *_table_Orders*进行特殊配置。
3. 新订单声音提醒。
4. 状态栏信息更新问题。通过以下方式初始化订单栏。

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

单击增加菜品按钮，加载*DialogAddRecord*界面类进行菜品添加。将图片文件转存到本地缓存。

###### 删

单击删除菜品按钮，判断是否选中行后，使用*QSqlTableModel*的`removeRow()`方法删除数据库记录。删除本地图片文件缓存。

###### 改

单击修改菜品按钮，判断是否选中行后，加载*DialogEditRecord*界面类进行菜品修改。若图片更改，删除旧的本地图片缓存，将新的图片转存到本地缓存。

##### 非核心的关键功能

###### 向客户端发送更新菜单消息

当服务端修改菜单数据库后，使用`generateUpdatedMenu()`方法生成菜单更新消息，发送至所有在线客户端。

#### 修改服务器配置

在*tab_Settings*中对服务器配置进行修改。主要有以下配置的修改：

* TCP设置
    * TCP服务端IP
    * TCP服务端菜单订单端口
    * TCP服务端状态信道端口

其中，配置储存在目录下的*config.json*文件中。

*config.json*文件内容如下：

```json
{
  "clearShot": 30,
  "tcpHost": "127.0.0.1",
  "tcpPort": 8081,
  "tcpStatusPort": 8082
}
```

### 客户端

客户端主要实现四个功能。菜单展示，购物车功能，发送订单信息和修改客户端配置。

客户端主要涉及Tcp操作以及Http操作。客户端使用Tcp实时与服务端进行信息交换，客户端使用http请求模拟来下载http服务器中的图片。

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

  

  

  