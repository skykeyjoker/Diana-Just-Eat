#ifndef DIALOGCARTVIEW_H
#define DIALOGCARTVIEW_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QDebug>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>

#include "cartitem.h"

class DialogCartView : public QDialog
{
    Q_OBJECT
public:
    explicit DialogCartView(const QList<CartItem>&cartlist, QWidget *parent = nullptr);

    QTableWidget *_cartTable;
    QLabel *lb_cartNum;
    QLabel *lb_cartNumContent;
    QLabel *lb_cartPrice;
    QLabel *lb_cartPriceContent;

private:
    QList<CartItem> _cartList;
    double _cartPrice = 0;
    int _cartNum = 0;
signals:
    void signalCartChanged(QList<CartItem>cartlist);
    void signalCartCleaned();
    void signalCartCheckOut();
public slots:
    void btnClearClicked();
    void btnCheckOutBtnClicked();
    void slotCartChanged(QTableWidgetItem *);


};

#endif // DIALOGCARTVIEW_H
