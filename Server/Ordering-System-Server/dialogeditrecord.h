#ifndef DIALOGEDITRECORD_H
#define DIALOGEDITRECORD_H

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTextBrowser>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QPixmap>
#include <QCloseEvent>
#include <QDir>
#include <QFileInfo>
#include <QFile>


//#include "convertpic.h"
#include "httpfileload.h"

class DialogEditRecord : public QDialog
{
    Q_OBJECT
public:
    explicit DialogEditRecord(QWidget *parent = nullptr);

    //void setValue(int dishId, QString dishName, QString dishType, QString dishInfo, QString dishPrice, QByteArray dishPhoto);
    void setValue(int dishId, QString dishName, QString dishType, QString dishInfo, QString dishPrice, QString dishPhoto, QString url);

    // dish info
    int _dishId;
    QString _dishName;
    QString _dishType;
    QString _dishInfo;
    QString _dishPrice;
    //QByteArray _dishPhoto;
    QString _dishPhoto;

    // widgets
    QLineEdit *le_Name = new QLineEdit;
    QLineEdit *le_Type = new QLineEdit;
    QTextEdit *le_Info = new QTextEdit;
    QLineEdit *le_Price = new QLineEdit;
    QLineEdit *le_Photo = new QLineEdit;
    QLabel *brower = new QLabel;

    // QPixmap
    QString picPath;
    QPixmap _pic;

    QString _url;

public slots:
    void slotBtnCancelClicked();
    void slotBtnSubmitClicked();

    void closeEvent(QCloseEvent *); //重写退出事件
signals:
    //void signalUpdate(int dishId, QString dishName, QString dishType, QString dishInfo, QString dishPrice, QByteArray dishPhoto);
    void signalUpdate(int dishId, QString dishName, QString dishType, QString dishInfo, QString dishPrice, QString dishPhoto);
};

#endif // DIALOGEDITRECORD_H
