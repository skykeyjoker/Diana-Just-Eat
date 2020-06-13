#include "myspinbox.h"

MySpinBox::MySpinBox(QWidget *parent) : QWidget(parent)
{

    lay = new QHBoxLayout(this);
    edit = new QLineEdit;
    btnAdd = new MyButton(":/Res/addtocart.png",QSize(25,25));
    btnMinus = new MyButton(":/Res/removefromcart.png",QSize(25,25));

    lay->addWidget(btnMinus);
    lay->addWidget(edit);
    lay->addWidget(btnAdd);


    edit->setValidator(new QIntValidator(0,100,this));
    edit->setReadOnly(true);

    btnAdd->setToolTip("增加一份菜品");
    btnMinus->setToolTip("减少一份菜品");

    connect(btnMinus,&QPushButton::clicked,this,&MySpinBox::slotBtnMinusClicked);
    connect(btnAdd,&QPushButton::clicked,this,&MySpinBox::slotBtnAddClicked);
    //connect(edit,&QLineEdit::textChange);
}

void MySpinBox::slotBtnMinusClicked()
{
    btnMinus->zoomUp();
    btnMinus->zoomDown();

    QString currentStr = edit->text();
    int currentNum = currentStr.toInt();

    if(currentNum-1<0)
    {
        qDebug()<<"Wrong, Number must be bigger than -1";
        QMessageBox::critical(this,"菜品数量错误！","菜品数量必须位于0~100之间！");
        return;
    }

    edit->setText(QString::number(currentNum-1));
}

void MySpinBox::slotBtnAddClicked()
{
    btnAdd->zoomUp();
    btnAdd->zoomDown();

    QString currentStr = edit->text();
    int currentNum = currentStr.toInt();

    if(currentNum+1>100)
    {
        qDebug()<<"Wrong, number must be smaller than 101";
        QMessageBox::critical(this,"菜品数量错误！","菜品数量必须位于0~100之间！");
        return;
    }

    edit->setText(QString::number(currentNum+1));
}

void MySpinBox::setText(const QString &str)
{
    edit->setText(str);
}

QString MySpinBox::text()
{
    return edit->text();
}

void MySpinBox::paintEvent(QPaintEvent *)
{
    QPalette pal = this->palette();
    QBrush brush = pal.background();
    QColor col = brush.color();
    QPainter painter(this);
    painter.setPen(col);
    painter.setBrush(brush);
    painter.drawRect(this->rect());
    //qDebug()<<col;
}
