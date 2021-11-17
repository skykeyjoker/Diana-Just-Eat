#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "json.hpp"

class DialogSettings : public QDialog {
	Q_OBJECT
public:
	explicit DialogSettings(const QString &tcpHost, const int tcpPort, const int tcpStatusPort, const QString &tableNum, QWidget *parent = nullptr);

	QLineEdit *le_TcpHost;
	QLineEdit *le_TcpPort;
	QLineEdit *le_TcpStatusPort;
	QLineEdit *le_TableNum;

signals:

private:
	QString _tcpHost;
	int _tcpPort;
	int _tcpStatusPort;
	QString _tableNum;

public slots:
	void slotUpdateBtnClicked();
	void slotRevBtnClicked();
};

#endif// DIALOGSETTINGS_H
