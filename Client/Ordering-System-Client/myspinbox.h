#ifndef MYSPINBOX_H
#define MYSPINBOX_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QIntValidator>
#include <QDebug>
#include <QPainter>
#include <QBrush>
#include <QPalette>
#include <QMessageBox>

#include "mybutton.h"

class MySpinBox : public QWidget
{
    Q_OBJECT
public:
    explicit MySpinBox(QWidget *parent = nullptr);
    QLineEdit *edit;
    MyButton *btnMinus;
    MyButton *btnAdd;
    QHBoxLayout *lay;

    void setText(const QString&);

    void paintEvent(QPaintEvent *ev);

    QString text();

signals:

public slots:
    //void slotEditTextChanged();
    void slotBtnMinusClicked();
    void slotBtnAddClicked();

};

#endif // MYSPINBOX_H
