#ifndef CARTITEM_H
#define CARTITEM_H

#include <QCoreApplication>
#include <QtCore>

class CartItem {
public:
	CartItem(QString itemName, double itemPrice) : _itemName(itemName),
												   _itemPrice(itemPrice) {
		_itemNum = 0;
	}

	void addItem();//增加一份

	void removeItem();//减少一份

	void setItemNums(int num);//设置菜品份数

	void setItemPrice(double price);

	QString getItemName() const;//获取菜品份数

	double getSum() const;//获取该种菜品总价

	int getNum() const;//获取该种菜品总数

private:
	QString _itemName;//菜品名
	double _itemPrice;//菜品价格
	int _itemNum;     //菜品数量
};

#endif// CARTITEM_H
