#include "servermainwindow.h"
#include "./ui_servermainwindow.h"

ServerMainWindow::ServerMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ServerMainWindow)
{
    ui->setupUi(this);
    setWindowTitle("自主点餐系统服务端");


    // 连接数据库
    bool dbRet = connectDb();
    if (!dbRet)
    {
        QMessageBox::critical(this, "服务端启动失败", "无法连接数据库");
        exit(1);
    }


    ui->tabWidget->setCurrentIndex(0);

    // Init Tab of Orders
    //TODO 订单tab
    QVBoxLayout *layOrders = new QVBoxLayout(ui->tab_Orders);
    _list_Orders = new QListWidget;
    layOrders->addWidget(_list_Orders);


    // Init Tab of Menu
    QVBoxLayout *layMenu = new QVBoxLayout(ui->tab_Menu);
    _view_Menu = new QTableView;
    layMenu->addWidget(_view_Menu);


    _model->setHeaderData(0, Qt::Horizontal, "菜品编号");
    _model->setHeaderData(1, Qt::Horizontal, "菜品名称");
    _model->setHeaderData(2, Qt::Horizontal, "菜品种类");
    _model->setHeaderData(3, Qt::Horizontal, "菜品描述");
    _model->setHeaderData(4, Qt::Horizontal, "菜品价格");
    _model->setHeaderData(5, Qt::Horizontal, "菜品图片");
    _view_Menu->setModel(_model);
    _view_Menu->setColumnHidden(0,true); //隐藏ID列
    _view_Menu->setAlternatingRowColors(true); //行颜色变换
    _view_Menu->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);  //让“描述”随拉伸变化
    _view_Menu->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);  //让“图片”随拉伸变化
    _view_Menu->setColumnWidth(3, 250);
    _view_Menu->setColumnWidth(5, 250);


    QHBoxLayout *layMenu_btn = new QHBoxLayout(NULL);
    QPushButton *btnEdit = new QPushButton("修改菜品");
    QPushButton *btnAdd = new QPushButton("添加菜品");
    QPushButton *btnDel = new QPushButton("删除菜品");
    layMenu_btn->addWidget(btnEdit);
    layMenu_btn->addSpacing(15);
    layMenu_btn->addWidget(btnAdd);
    layMenu_btn->addSpacing(15);
    layMenu_btn->addWidget(btnDel);
    layMenu->addLayout(layMenu_btn);


    //绑定按钮信号与槽
    connect(btnEdit, &QPushButton::clicked, this, &ServerMainWindow::slotBtnEditClicked);
    connect(btnAdd, &QPushButton::clicked, this, &ServerMainWindow::slotBtnAddClicked);
    connect(btnDel, &QPushButton::clicked, this, &ServerMainWindow::slotBtnDelClicked);


    // Init Tab for Config
    //TODO 服务器配置tab
}

ServerMainWindow::~ServerMainWindow()
{
    delete ui;
}

bool ServerMainWindow::connectDb()
{
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

    //设置QSqlModel
    _model = new QSqlTableModel;
    _model->setTable("menu");
    _model->select();
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit); //设置提交策略

    return true;
}

void ServerMainWindow::slotBtnEditClicked()
{
    if (_view_Menu->currentIndex().row() == -1)
    {
        QMessageBox::critical(this, "编辑失败", "未选择任何行");
        return;
    }

    DialogEditRecord *dlg = new DialogEditRecord;

    QSqlRecord record = _model->record(_view_Menu->currentIndex().row());
    int dishId = record.value(0).toInt();
    QString dishName = record.value(1).toString();
    QString dishType = record.value(2).toString();
    QString dishInfo = record.value(3).toString();
    QString dishPrice = record.value(4).toString();
    QByteArray dishPhoto =record.value(5).toByteArray();

    dlg->setValue(dishId,dishName,dishType,dishInfo,dishPrice,dishPhoto);
    dlg->show();


    //利用函数指针调用DialogEditRecord带参数的signalUpdate信号，和ServerMainWindow带参数的slotUpdate槽
    void (DialogEditRecord::*pSignalUpdate)(int, QString, QString, QString, QString, QByteArray)=&DialogEditRecord::signalUpdate;
    void (ServerMainWindow::*pSlotUpdate)(int, QString, QString, QString, QString, QByteArray)=&ServerMainWindow::slotUpdate;

    connect(dlg,pSignalUpdate,this,pSlotUpdate);
}

void ServerMainWindow::slotBtnAddClicked()
{
    DialogAddRecord *dlg = new DialogAddRecord;
    dlg->show();

    //利用函数指针调用DialogAddRecord带参数的signalSubmit信号，和ServerMainWindow带参数的slotSubmit槽
    void (DialogAddRecord::*pSignalSubmit)(QString, QString, QString, QString, QByteArray)=&DialogAddRecord::signalSubmit;
    void (ServerMainWindow::*pSlotSubmit)(QString, QString, QString, QString, QByteArray)=&ServerMainWindow::slotSubmit;

    connect(dlg,pSignalSubmit,this,pSlotSubmit);
}

void ServerMainWindow::slotBtnDelClicked()
{
    if (_view_Menu->currentIndex().row() == -1)
    {
        QMessageBox::critical(this, "删除失败", "未选择任何行");
        return;
    }

    int ret = QMessageBox::question(this, "确认删除", "你确认要删除这个菜品吗？");
    if (ret == QMessageBox::Yes)
    {
        bool delRet = _model->removeRow(_view_Menu->currentIndex().row());
        if (!delRet)
        {
            QMessageBox::critical(this, "删除失败", "删除菜品失败！");
            qDebug() << db.lastError().text();
            return;
        }
        bool submitRet = _model->submitAll();
        if (!submitRet)
        {
            QMessageBox::critical(this, "删除失败", "删除菜品失败！");
            qDebug() << db.lastError().text();
            return;
        }
    }
}

void ServerMainWindow::slotSubmit(QString dishName, QString dishType, QString dishInfo, QString dishPrice, QByteArray dishPhoto)
{
    qDebug()<<"slotSubmit";
    //qDebug()<<dishName<<" "<<dishType<<" "<<dishInfo<<" "<<dishPrice<<" "<<dishPhoto;

    QSqlRecord record = _model->record();
    record.setValue(1,dishName);
    record.setValue(2,dishType);
    record.setValue(3,dishInfo);
    record.setValue(4,dishPrice.toInt());
    record.setValue(5,QString::fromUtf8(dishPhoto));

    bool bRet = _model->insertRecord(-1,record);

    if(!bRet)
    {
        QMessageBox::critical(this,"添加失败","菜品添加失败！");
        qDebug()<<"add error "<<db.lastError().text();
        return;
    }
    else qDebug()<<"insert ok";

    _model->submitAll();
}

void ServerMainWindow::slotUpdate(int dishId, QString dishName, QString dishType, QString dishInfo, QString dishPrice, QByteArray dishPhoto)
{
    qDebug()<<"slotUpdate";
    //qDebug()<<dishId<<" "<<dishName<<" "<<dishType<<" "<<dishInfo<<" "<<dishPrice<<" "<<dishPhoto;
    QSqlRecord record = _model->record(_view_Menu->currentIndex().row());
    record.setValue(1,dishName);
    record.setValue(2,dishType);
    record.setValue(3,dishInfo);
    record.setValue(4,dishPrice.toInt());
    if(dishPhoto!=record.value(5))  //检测图片是否更新
    {
        record.setValue(5,QString::fromUtf8(dishPhoto));
        qDebug()<<"图片改变";
    }

    bool bRet = _model->setRecord(_view_Menu->currentIndex().row(),record);

    if(!bRet)
    {
        QMessageBox::critical(this,"修改失败","菜品修改失败！");
        qDebug()<<"update error "<<db.lastError().text();
        return;
    }
    else qDebug()<<"update ok";

    _model->submitAll();
}

//重写退出事件
void ServerMainWindow::closeEvent(QCloseEvent *event)
{
    int ret = QMessageBox::question(this,"确认取消","您确认要退出服务端吗？");

    if(ret == QMessageBox::Yes)
    {
        event->accept();
    }
    else
        event->ignore();
}
