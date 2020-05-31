#include "dialogordersviewer.h"

DialogOrdersViewer::DialogOrdersViewer(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("订单查看");
    setWindowIcon(QIcon(":/Res/at8.ico"));



    QVBoxLayout *lay = new QVBoxLayout(this);

    QHBoxLayout *hlayStatus = new QHBoxLayout;
    hlayStatus->addWidget(lb_Status);
    hlayStatus->addWidget(lb_Status_Content);

    QHBoxLayout *hlayOrderNum = new QHBoxLayout;
    hlayOrderNum->addWidget(lb_OrderNum);
    hlayOrderNum->addWidget(le_OrderNum);

    QHBoxLayout *hlayTableNum = new QHBoxLayout;
    hlayTableNum->addWidget(lb_TableNum);
    hlayTableNum->addWidget(le_TableNum);

    QHBoxLayout *hlayOrderPrice = new QHBoxLayout;
    hlayOrderPrice->addWidget(lb_OrderPrice);
    hlayOrderPrice->addWidget(le_OrderPrice);

    QHBoxLayout *hlayOrderContent = new QHBoxLayout;
    hlayOrderContent->addWidget(lb_OrderContent);
    hlayOrderContent->addWidget(_table);

    QHBoxLayout *hlayOrderNote = new QHBoxLayout;
    hlayOrderNote->addWidget(lb_OrderNote);
    hlayOrderNote->addWidget(tb_OrderNote);


    lay->addLayout(hlayStatus);
    lay->addLayout(hlayOrderNum);
    lay->addLayout(hlayTableNum);
    lay->addLayout(hlayOrderPrice);
    lay->addLayout(hlayOrderContent);
    lay->addLayout(hlayOrderNote);

    //设置_table属性
    _table->setColumnCount(2);
    _table->setHorizontalHeaderLabels(QStringList()<<"菜品"<<"数量");
    _table->setColumnWidth(0,230);
    _table->setColumnWidth(1,100);
    _table->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行行为，以行为单位
    _table->setSelectionMode(QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置为禁止编辑

    //设置其他控件属性
    le_OrderNum->setReadOnly(true);
    le_TableNum->setReadOnly(true);
    le_OrderPrice->setReadOnly(true);
    tb_OrderNote->setReadOnly(true);
}


void DialogOrdersViewer::setData(bool isHandled, QString orderNum, QString tableNum, QString orderPrice, QString orderContent, QString orderNote)
{
    //le_OrderNum->setText("123");

    //设置控件内容
    if(isHandled)
    {
        lb_Status_Content->setText("已处理");
    }
    else
        lb_Status_Content->setText("未处理");

    le_OrderNum->setText(orderNum);
    le_TableNum->setText(tableNum);
    le_OrderPrice->setText(orderPrice);
    tb_OrderNote->setText(orderNote);
    qDebug()<<"即将处理table";

    //_table
    int sectionNums = orderContent.count(',')+1;
    qDebug()<<sectionNums;

    for(int i=0;i<sectionNums;i++)
    {
        _table->setRowCount(_table->rowCount()+1);
        int currentRow = _table->rowCount();

        QString rawStr = orderContent.section(',',i,i);
        qDebug()<<rawStr;
        rawStr = rawStr.remove(0,1);
        rawStr = rawStr.remove(rawStr.length()-1,1);
        qDebug()<<rawStr;

        QString name = rawStr.section(':',0,0);
        QString num = rawStr.section(':',1,1);

        _table->setItem(currentRow-1,0,new QTableWidgetItem(name));
        _table->setItem(currentRow-1,1,new QTableWidgetItem(num));
    }

}
