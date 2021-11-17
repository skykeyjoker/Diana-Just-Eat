#ifndef DIALOGEDITRECORD_H
#define DIALOGEDITRECORD_H

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


class DialogEditRecord : public QDialog {
	Q_OBJECT
public:
	explicit DialogEditRecord(QWidget *parent = nullptr);

	//void setValue(int dishId, QString dishName, QString dishType, QString dishInfo, QString dishPrice, QByteArray dishPhoto);
	//void setValue(int dishId, QString dishName, QString dishType, QString dishInfo, QString dishPrice, QString dishPhoto, QString url);
	void setValue(int dishId, const QString &dishName, const QString &dishType, const QString &dishInfo, const double dishPrice, const QString &dishPhoto);

private:
	// dish info
	int _dishId;
	QString _dishName;
	QString _dishType;
	QString _dishInfo;
	double _dishPrice;
	//QByteArray _dishPhoto;
	QString _dishPhoto;

	// widgets
	QLineEdit *le_Name;
	QLineEdit *le_Type;
	QTextEdit *le_Info;
	QLineEdit *le_Price;
	QLineEdit *le_Photo;
	QLabel *brower;

	// QPixmap
	QString picPath;
	QPixmap _pic;

public slots:
	void slotBtnCancelClicked();
	void slotBtnSubmitClicked();

	void closeEvent(QCloseEvent *);//重写退出事件
signals:
	//void signalUpdate(int dishId, QString dishName, QString dishType, QString dishInfo, QString dishPrice, QByteArray dishPhoto);
	void signalUpdate(int dishId, const QString dishName, const QString dishType, const QString dishInfo, const double dishPrice, const QString dishPhoto, bool photoUpdated);
};

#endif// DIALOGEDITRECORD_H
