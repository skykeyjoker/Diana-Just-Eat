#ifndef DIALOGADDRECORD_H
#define DIALOGADDRECORD_H

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
#include <QFile>
#include <QFileInfo>


//#include "convertpic.h"
#include "httpfileload.h"
#include "httpfileupdate.h"


class DialogAddRecord : public QDialog
{
    Q_OBJECT
public:
    explicit DialogAddRecord(QWidget *parent = nullptr);

    // dish info
    QString _dishName;
    QString _dishType;
    QString _dishInfo;
    QString _dishPrice;
    //QByteArray _dishPhoto;
    QString _dishPhoto;

    // widgets
    QLineEdit *le_Name;
    QLineEdit *le_Type;
    QTextEdit *le_Info;
    QLineEdit *le_Price;
    QLineEdit *le_Photo;

    // pixmap
    QString picPath;
    QPixmap _pix;

    QString _url;

    void setUrl(QString url);
public slots:
    void slotBtnCancelClicked();
    void slotBtnSubmitClicked();

    void closeEvent(QCloseEvent *); //重写退出事件
signals:
    //void signalSubmit(QString dishName, QString dishType, QString dishInfo, QString dishPrice, QByteArray dishPhoto);
    void signalSubmit(QString dishName, QString dishType, QString dishInfo, QString dishPrice, QString dishPhoto);
};

#endif // DIALOGADDRECORD_H
