#ifndef CARTITEM_H
#define CARTITEM_H

#include <QtCore>
#include <QCoreApplication>

class CartItem
{
public:
    CartItem(QString itemName, double itemPrice):_itemName(itemName), _itemPrice(itemPrice)
    {
        _itemNum=0;
    }

    void addItem();

    void removeItem();

    void setItemNums(int num);

    QString getItemName() const;

    double getSum() const;

    int getNum() const;

private:
    QString _itemName;
    double _itemPrice;
    int _itemNum;

};

#endif // CARTITEM_H
