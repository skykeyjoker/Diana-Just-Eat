#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QEvent>
#include <QMouseEvent>


class MyButton : public QPushButton
{
    Q_OBJECT
public:
    MyButton(QString pixMapName, QSize buttonSize);


    void zoomUp();
    void zoomDown();

public slots:
    void showAddAnimation();

signals:

};

#endif // MYBUTTON_H
