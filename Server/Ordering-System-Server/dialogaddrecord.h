#ifndef DIALOGADDRECORD_H
#define DIALOGADDRECORD_H

#include <QCloseEvent>
#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QTextBrowser>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>


//#include "convertpic.h"
#include "httpfileload.h"
#include "httpfileupdate.h"


class DialogAddRecord : public QDialog {
	Q_OBJECT
public:
	explicit DialogAddRecord(QWidget *parent = nullptr);

	// dish info
	QString _dishName;
	QString _dishType;
	QString _dishInfo;
	double _dishPrice;
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
	QLabel *browser;

public slots:
	void slotBtnCancelClicked();
	void slotBtnSubmitClicked();

	void closeEvent(QCloseEvent *);//重写退出事件
signals:
	//void signalSubmit(QString dishName, QString dishType, QString dishInfo, QString dishPrice, QByteArray dishPhoto);
	void signalSubmit(QString dishName, QString dishType, QString dishInfo, double dishPrice, QString dishPhoto);
};

#endif// DIALOGADDRECORD_H
