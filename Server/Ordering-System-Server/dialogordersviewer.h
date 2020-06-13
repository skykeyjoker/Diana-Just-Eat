#ifndef DIALOGORDERSVIEWER_H
#define DIALOGORDERSVIEWER_H

#include <QWidget>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QLabel>
#include <QHeaderView>
#include <QDebug>


class DialogOrdersViewer : public QDialog
{
    Q_OBJECT
public:
    explicit DialogOrdersViewer(QWidget *parent = nullptr);

    //界面控件
    QLabel *lb_Status = new QLabel("订单状态：");
    QLabel *lb_Status_Content = new QLabel;
    QLabel *lb_OrderNum = new QLabel("订单号：");
    QLineEdit *le_OrderNum = new QLineEdit;
    QLabel *lb_TableNum = new QLabel("订单桌号：");
    QLineEdit *le_TableNum = new QLineEdit;
    QLabel *lb_OrderPrice = new QLabel("订单总价格：");
    QLineEdit *le_OrderPrice = new QLineEdit;
    QLabel *lb_OrderContent = new QLabel("订单内容：");
    QTableWidget *_table = new QTableWidget;
    QLabel *lb_OrderNote = new QLabel("订单备注：");
    QTextBrowser *tb_OrderNote = new QTextBrowser;

    //设置信息
    void setData(bool isHandled, QString orderNum, QString tableNum, QString orderPrice, QString orderContent, QString orderNote);

signals:

};

#endif // DIALOGORDERSVIEWER_H
