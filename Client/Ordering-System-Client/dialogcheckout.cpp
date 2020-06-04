#include "dialogcheckout.h"

DialogCheckOut::DialogCheckOut(const QList<CartItem>&cartlist, int totalNum, int totalPrice, QWidget *parent) : QDialog(parent)
{
    this->setWindowIcon(QIcon(":/Res/at8.ico"));
    this->setWindowTitle("订单确认");
    this->setFixedSize(600,600);

    QVBoxLayout *lay = new QVBoxLayout(this);
    QTableWidget *table = new QTableWidget;
    QLabel *lb_totalNum = new QLabel("购物车菜品总数：");
    QLabel *lb_totalNumContent = new QLabel(QString::number(totalNum));
    QLabel *lb_totalPrice = new QLabel("购物车菜品总价：");
    QLabel *lb_totalPriceContent = new QLabel(QString::number(totalPrice));
    QPushButton *btn_cancel = new QPushButton("取消");
    btn_cancel->setIcon(QIcon(":/Res/cancel.png")); btn_cancel->setIconSize(QSize(40,40));
    QPushButton *btn_checkout = new QPushButton("确定");
    btn_checkout->setIcon(QIcon(":/Res/confirm.png")); btn_checkout->setIconSize(QSize(40,40));
    QLabel *lb_note = new QLabel("订单备注：");
    _textNote = new QTextEdit;

    QHBoxLayout *layNum = new QHBoxLayout;
    layNum->addWidget(lb_totalNum);
    layNum->addWidget(lb_totalNumContent);

    QHBoxLayout *layPrice = new QHBoxLayout;
    layPrice->addWidget(lb_totalPrice);
    layPrice->addWidget(lb_totalPriceContent);

    QHBoxLayout *layNote = new QHBoxLayout;
    layNote->addWidget(lb_note);
    layNote->addWidget(_textNote);

    QHBoxLayout *layBtns = new QHBoxLayout;
    layBtns->addStretch(1);
    layBtns->addWidget(btn_cancel);
    layBtns->addStretch(2);
    layBtns->addWidget(btn_checkout);
    layBtns->addStretch(1);


    lay->addWidget(table);
    lay->addLayout(layNum);
    lay->addLayout(layPrice);
    lay->addLayout(layNote);
    lay->addLayout(layBtns);

    //填充table
    //配置tableWidget内容
    QStringList tableHeaders;
    tableHeaders<<"菜品名"<<"数量"<<"总价格";
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(tableHeaders);
    //让三列随拉伸变化
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置为只读
    table->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置为选择单位为行
    table->setSelectionMode(QAbstractItemView::SingleSelection); //设置选择一行

    //增加table内容
    for(int i=0;i<cartlist.size();i++)
    {
        QTableWidgetItem *currentItemName = new QTableWidgetItem;
        QTableWidgetItem *currentItemNum = new QTableWidgetItem;
        QTableWidgetItem *currentItemPrice = new QTableWidgetItem;
        currentItemName->setText(cartlist.at(i).getItemName());
        currentItemNum->setText(QString::number(cartlist.at(i).getNum()));
        currentItemPrice->setText(QString::number(cartlist.at(i).getSum()));

        //插入新行
        table->setRowCount(table->rowCount()+1);
        //为新行赋值
        int currentRow = table->rowCount()-1;
        table->setItem(currentRow,0,currentItemName);
        table->setItem(currentRow,1,currentItemNum);
        table->setItem(currentRow,2,currentItemPrice);
    }


    //绑定按钮事件
    connect(btn_cancel,&QPushButton::clicked,[=](){
        int ret = QMessageBox::question(this,"请求确认","您确认要取消订单吗？");
        if(ret == QMessageBox::Yes)
        {
            this->close();
        }
    });

    connect(btn_checkout,&QPushButton::clicked,[=](){
        int ret = QMessageBox::question(this,"请求确认","您确认要提交此订单吗？");
        if(ret == QMessageBox::Yes)
        {
            emit signalReadyCheckOut(_textNote->toPlainText());
            this->close();
        }
    });
}
