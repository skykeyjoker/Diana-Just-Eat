#include "mydatetimeselect.h"

MyDateTimeSelect::MyDateTimeSelect(QWidget *parent) : QWidget(parent)
{
    lay = new QVBoxLayout(this);
    editLay = new QHBoxLayout;
    btn = new MyButton(":/Res/calendar.png",QSize(30,30));
    edit = new QDateTimeEdit;
    calendar = new QCalendarWidget;

    editLay->addWidget(edit);
    editLay->addWidget(btn);

    lay->addLayout(editLay);
    lay->addWidget(calendar);

    calendar->setVisible(false);

    edit->setDateTime(QDateTime::currentDateTime());

    connect(btn,&QPushButton::clicked,[=](){
        calendar->setVisible(true);
    });

    void (QCalendarWidget::*pSignalclicked)(const QDate&) = &QCalendarWidget::clicked;
    void (MyDateTimeSelect::*pSlotDateChoose)(const QDate&) = &MyDateTimeSelect::slotDateChoose;

    connect(calendar,pSignalclicked,this,pSlotDateChoose);
}

void MyDateTimeSelect::slotDateChoose(const QDate& date)
{
    edit->setDate(date);
    calendar->setVisible(false);
}

QDateTime MyDateTimeSelect::dateTime()
{
    return edit->dateTime();
}
