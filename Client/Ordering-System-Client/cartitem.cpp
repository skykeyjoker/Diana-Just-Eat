#include "cartitem.h"

double CartItem::getSum() const {
	return _itemPrice * _itemNum;
}

void CartItem::addItem() {
	_itemNum++;
}

void CartItem::removeItem() {
	_itemNum--;
}

QString CartItem::getItemName() const {
	return _itemName;
}

int CartItem::getNum() const {
	return _itemNum;
}

void CartItem::setItemNums(int num) {
	_itemNum = num;
}

void CartItem::setItemPrice(double price) {
	_itemPrice = price;
}
