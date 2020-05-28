#include "servermainwindow.h"
#include "./ui_servermainwindow.h"

ServerMainWindow::ServerMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerMainWindow)
{
    ui->setupUi(this);
    setWindowTitle("自主点餐系统服务端");

    // 连接数据库
    bool dbRet = connectDb();
    if(!dbRet)
    {
        QMessageBox::critical(this,"服务端启动失败","无法连接数据库");
        exit(1);
    }


    ui->tabWidget->setCurrentIndex(0);


    // Init Tab of Orders
    QVBoxLayout *layOrders = new QVBoxLayout(ui->tab_Orders);
    _list_Orders =  new QListWidget;
    layOrders->addWidget(_list_Orders);


    // Init Tab of Menu
    QVBoxLayout *layMenu = new QVBoxLayout(ui->tab_Menu);
    _view_Menu = new QTableView;
    layMenu->addWidget(_view_Menu);

    _model->setHeaderData(0,Qt::Horizontal,"菜品编号");
    _model->setHeaderData(1,Qt::Horizontal,"菜品名称");
    _model->setHeaderData(2,Qt::Horizontal,"菜品种类");
    _model->setHeaderData(3,Qt::Horizontal,"菜品描述");
    _model->setHeaderData(4,Qt::Horizontal,"菜品价格");
    _model->setHeaderData(5,Qt::Horizontal,"菜品图片");
    _view_Menu->setModel(_model);
    _view_Menu->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    _view_Menu->horizontalHeader()->setSectionResizeMode(5,QHeaderView::Stretch);
    _view_Menu->setColumnWidth(3,250);
    _view_Menu->setColumnWidth(5,250);

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
    connect(btnEdit,&QPushButton::clicked,this,&ServerMainWindow::slotBtnEditClicked);
    connect(btnAdd,&QPushButton::clicked,this,&ServerMainWindow::slotBtnAddClicked);
    connect(btnDel,&QPushButton::clicked,this,&ServerMainWindow::slotBtnDelClicked);

}

ServerMainWindow::~ServerMainWindow()
{
    delete ui;
}

bool ServerMainWindow::connectDb()
{
    bool isOk = dbInfo.readFromFile("./config.json");

    if(!isOk)
    {
        qDebug()<<"无法读取配置文件";
        QMessageBox::critical(this,"启动失败","服务端启动失败，无法读取配置文件。");
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

    if(!dbRet)
    {
        qDebug()<<"数据库连接失败 "<<db.lastError().text();
        return false;
    }
    else
        qDebug()<<"数据库连接成功";


    //设置QSqlModel
    _model = new QSqlTableModel;
    _model->setTable("menu");
    _model->select();
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit); //设置提交策略

    return true;
}

void ServerMainWindow::slotBtnEditClicked()
{
    if(_view_Menu->currentIndex().row()==-1)
    {
        QMessageBox::critical(this,"编辑失败","未选择任何行");
        return;
    }



    DialogEditRecord *dlg = new DialogEditRecord;
    dlg->exec();
}

void ServerMainWindow::slotBtnAddClicked()
{
    QSqlRecord record;

    DialogAddRecord *dlg = new DialogAddRecord;
    dlg->exec();
    //record.setValue();

    //_model->insertRecord(-1,record);
}

void ServerMainWindow::slotBtnDelClicked()
{
    if(_view_Menu->currentIndex().row()==-1)
    {
        QMessageBox::critical(this,"删除失败","未选择任何行");
        return;
    }

    int ret = QMessageBox::question(this,"确认删除","你确认要删除这个菜品吗？");
    if(ret == QMessageBox::Yes)
    {
        bool delRet = _model->removeRow(_view_Menu->currentIndex().row());
        if(!delRet)
        {
            QMessageBox::critical(this,"删除失败","删除菜品失败！");
            qDebug()<<db.lastError().text();
            return;
        }
        bool submitRet = _model->submitAll();
        if(!submitRet)
        {
            QMessageBox::critical(this,"删除失败","删除菜品失败！");
            qDebug()<<db.lastError().text();
            return;
        }
    }
}
