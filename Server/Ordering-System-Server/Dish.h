#ifndef ORDERING_SYSTEM_DISH_H
#define ORDERING_SYSTEM_DISH_H

#include <QtCore>

class Dish {
public:
	explicit Dish(const QString &name, const QString &type, const QString &info, const double price, const QString &photo);
	~Dish();

public:
	void setName(const QString &name);
	void setType(const QString &type);
	void setInfo(const QString &info);
	void setPrice(const double price);
	void setPhoto(const QString &photo);

public:
	QString getName();
	QString getType();
	QString getInfo();
	double getPrice();
	QString getPhoto();

private:
	QString _name;
	QString _type;
	QString _info;
	double _price;
	QString _photo;
};


#endif//ORDERING_SYSTEM_DISH_H
