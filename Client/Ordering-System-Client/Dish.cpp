#include "Dish.h"

Dish::Dish(const QString& name, const QString& type, const QString& info, const double price, const QString& photo)
	: _name(name),
	  _type(type),
	  _info(info),
	  _price(price),
	  _photo(photo) {
}

Dish::~Dish() {
}

void Dish::setName(const QString& name) {
	_name = name;
}

void Dish::setType(const QString& type) {
	_type = type;
}

void Dish::setInfo(const QString& info) {
	_info = info;
}

void Dish::setPrice(const double price) {
	_price = price;
}

void Dish::setPhoto(const QString& photo) {
	_photo = photo;
}

QString Dish::getName() {
	return _name;
}

QString Dish::getType() {
	return _type;
}

QString Dish::getInfo() {
	return _info;
}

double Dish::getPrice() {
	return _price;
}

QString Dish::getPhoto() {
	return _photo;
}

QString Dish::serialize() {
	return QString();
}
