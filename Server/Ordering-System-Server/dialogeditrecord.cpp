#include "dialogeditrecord.h"

DialogEditRecord::DialogEditRecord(QWidget *parent) : QDialog(parent)
{
    setFixedSize(800, 500);
    setWindowTitle("修改菜品信息");


    QVBoxLayout *vlay = new QVBoxLayout(this);

    QHBoxLayout *hlay_Name = new QHBoxLayout(NULL);
    QLabel *lb_Name = new QLabel("菜品名称：");
    hlay_Name->addWidget(lb_Name);
    hlay_Name->addWidget(le_Name);

    QHBoxLayout *hlay_Type = new QHBoxLayout(NULL);
    QLabel *lb_Type = new QLabel("菜品种类：");
    hlay_Type->addWidget(lb_Type);
    hlay_Type->addWidget(le_Type);

    QHBoxLayout *hlay_Info = new QHBoxLayout(NULL);
    QLabel *lb_Info = new QLabel("菜品描述：");
    hlay_Info->addWidget(lb_Info);
    hlay_Info->addWidget(le_Info);

    QHBoxLayout *hlay_Price = new QHBoxLayout(NULL);
    QLabel *lb_Price = new QLabel("菜品价格：");
    hlay_Price->addWidget(lb_Price);
    hlay_Price->addWidget(le_Price);

    QHBoxLayout *hlay_Photo = new QHBoxLayout(NULL);
    QLabel *lb_Photo = new QLabel("菜品图片：");
    QPushButton *btn_Photo = new QPushButton("选择图片");
    hlay_Photo->addWidget(lb_Photo);
    hlay_Photo->addWidget(le_Photo);
    hlay_Photo->addSpacing(20);
    hlay_Photo->addWidget(btn_Photo);

    QHBoxLayout *hlay_Btn = new QHBoxLayout(NULL);
    QPushButton *btnCancel = new QPushButton("取消");
    QPushButton *btnSubmit = new QPushButton("提交");
    hlay_Btn->addSpacing(25);
    hlay_Btn->addWidget(btnCancel);
    hlay_Btn->addSpacing(100);
    hlay_Btn->addWidget(btnSubmit);
    hlay_Btn->addSpacing(25);

    QHBoxLayout *hlay_Pre = new QHBoxLayout(NULL);
    hlay_Pre->addSpacing(150);
    hlay_Pre->addWidget(brower);
    hlay_Pre->addSpacing(150);

    vlay->addLayout(hlay_Name);
    vlay->addLayout(hlay_Type);
    vlay->addLayout(hlay_Info);
    vlay->addLayout(hlay_Price);
    vlay->addLayout(hlay_Photo);
    vlay->addSpacing(10);
    vlay->addLayout(hlay_Pre);
    vlay->addSpacing(25);
    vlay->addLayout(hlay_Btn);



    connect(btn_Photo, &QPushButton::clicked, [=]() {
        picPath = QFileDialog::getOpenFileName(this, "选择菜品图片", "", "图片文件 (*.png *.jpg *.jpeg)");
        if (picPath.isEmpty())
        {
            QMessageBox::critical(this, "错误", "打开文件失败");
        }
        else
            le_Photo->setText(picPath);

        //预览图片
        _pic=QPixmap(picPath);
        brower->setPixmap(_pic);
    });


    //取消按钮
    connect(btnCancel,&QPushButton::clicked,this,&DialogEditRecord::slotBtnCancelClicked);

    //提交按钮
    connect(btnSubmit,&QPushButton::clicked,this,&DialogEditRecord::slotBtnSubmitClicked);
}

void DialogEditRecord::setValue(int dishId, QString dishName, QString dishType, QString dishInfo, QString dishPrice, QByteArray dishPhoto)
{
    _dishId = dishId;
    _dishName = dishName;
    _dishType = dishType;
    _dishInfo = dishInfo;
    _dishPrice = dishPrice;
    _dishPhoto = dishPhoto;


    // 设置内容
    le_Name->setText(_dishName);
    le_Type->setText(_dishType);
    le_Info->setText(_dishInfo);
    le_Price->setText(_dishPrice);

    _pic = toQPixmap(dishPhoto);

    brower->setPixmap(_pic);

    /*
    qDebug()<<_dishId;
    qDebug()<<_dishName;
    qDebug()<<_dishType;
    qDebug()<<_dishInfo;
    qDebug()<<_dishPrice;
    qDebug()<<_dishPhoto;
*/
}

void DialogEditRecord::slotBtnCancelClicked()
{
    close();
}

void DialogEditRecord::slotBtnSubmitClicked()
{
    if(le_Info->toPlainText().isEmpty()||le_Name->text().isEmpty()||le_Type->text().isEmpty()||le_Price->text().isEmpty())
    {
        QMessageBox::critical(this,"提交失败","关键信息不完整，请填写必要信息后再提交！");
        return;
    }

    _dishName = le_Name->text();
    _dishType = le_Type->text();
    _dishInfo = le_Info->toPlainText();
    _dishPrice = le_Price->text();
    if(!le_Photo->text().isEmpty())
    {
        _dishPhoto = toBase64(_pic);
    }

    emit signalUpdate(_dishId,_dishName,_dishType,_dishInfo,_dishPrice,_dishPhoto);
    QMessageBox::information(this,"成功","修改菜品信息成功！");
}

//重写退出事件
void DialogEditRecord::closeEvent(QCloseEvent *event)
{
    int ret = QMessageBox::question(this,"确认取消","您确认要退出添加菜品吗？");

    if(ret == QMessageBox::Yes)
    {
        event->accept();
    }
    else
        event->ignore();
}