#include "mybutton.h"

MyButton::MyButton(QString pixMapName, QSize buttonSize) : QPushButton(0)
{
    QPixmap pixmap(pixMapName);

    if(buttonSize.isEmpty())
    {
        this->setFixedSize(QSize(pixmap.width(),pixmap.height()));

        this->setStyleSheet("QPushButton{border:0px}");

        this->setIcon(pixmap);
        this->setIconSize(buttonSize);
    }
    else
    {
        this->setFixedSize(buttonSize.width(),buttonSize.height());

        this->setStyleSheet("QPushButton{border:0px}");

        this->setIcon(pixmap);
        this->setIconSize(buttonSize);
    }
}

void MyButton::zoomUp()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    animation->setDuration(200);
    animation->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
    animation->setEndValue(QRect(this->x(),this->y()-10,this->width(),this->height()));
    animation->setEasingCurve(QEasingCurve::OutBounce);

    animation->start();
}

void MyButton::zoomDown()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    animation->setDuration(200);
    animation->setStartValue(QRect(this->x(),this->y()-10,this->width(),this->height()));
    animation->setEndValue(QRect(this->x(),this->y(),this->width(),this->height()));
    animation->setEasingCurve(QEasingCurve::OutBounce);

    animation->start();
}

void MyButton::showAddAnimation()
{
    zoomUp();
    zoomDown();
}
