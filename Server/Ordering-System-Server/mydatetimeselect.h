#ifndef MYDATETIMESELECT_H
#define MYDATETIMESELECT_H

#include <QWidget>
#include <QDateTimeEdit>
#include <QDateTime>
#include <QCalendarWidget>
#include <QCalendar>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDate>

#include "mybutton.h"

class MyDateTimeSelect : public QWidget
{
    Q_OBJECT
public:
    explicit MyDateTimeSelect(QWidget *parent = nullptr);
    MyButton *btn;
    QCalendarWidget *calendar;
    QDateTimeEdit *edit;
    QVBoxLayout *lay;
    QHBoxLayout *editLay;

    QDateTime dateTime();

signals:

public slots:
    void slotDateChoose(const QDate&);

};

#endif // MYDATETIMESELECT_H
