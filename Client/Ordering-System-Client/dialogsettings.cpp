#include "dialogsettings.h"

DialogSettings::DialogSettings(const QString &tcpHost, const int tcpPort, const int tcpStatusPort, const QString &tableNum, QWidget *parent)
	: QDialog(parent),
	  _tcpHost(tcpHost),
	  _tcpPort(tcpPort),
	  _tcpStatusPort(tcpStatusPort),
	  _tableNum(tableNum) {
	this->setWindowTitle("客户端设置");
	this->setWindowIcon(QIcon(":/Res/settings.png"));


	/* 初始化界面 */
	QVBoxLayout *layConfig = new QVBoxLayout(this);

	QGroupBox *groupTcp = new QGroupBox("TCP设置");
	QVBoxLayout *layTcp = new QVBoxLayout(groupTcp);
	QLabel *lb_TcpHost = new QLabel("TCP服务端IP：");
	le_TcpHost = new QLineEdit;
	QLabel *lb_TcpPort = new QLabel("TCP服务端端口：");
	le_TcpPort = new QLineEdit;
	le_TcpStatusPort = new QLineEdit;

	QHBoxLayout *layTcpHost = new QHBoxLayout;
	layTcpHost->addWidget(lb_TcpHost);
	layTcpHost->addWidget(le_TcpHost);
	layTcpHost->addStretch(1);
	QHBoxLayout *layTcpPort = new QHBoxLayout;
	layTcpPort->addWidget(lb_TcpPort);
	layTcpPort->addWidget(le_TcpPort);
	layTcpPort->addWidget(le_TcpStatusPort);
	layTcpPort->addStretch(8);

	layTcp->addLayout(layTcpHost);
	layTcp->addLayout(layTcpPort);


	QGroupBox *groupTable = new QGroupBox("桌号设置：");
	QHBoxLayout *layTable = new QHBoxLayout(groupTable);
	QLabel *lb_TableNum = new QLabel("桌号：");
	le_TableNum = new QLineEdit;
	layTable->addWidget(lb_TableNum);
	layTable->addWidget(le_TableNum);
	layTable->addStretch(1);

	QHBoxLayout *layConfigBtns = new QHBoxLayout;
	QPushButton *btnRevConfig = new QPushButton("恢复设置");
	QPushButton *btnUpdateConfig = new QPushButton("更新设置");
	layConfigBtns->addStretch(2);
	layConfigBtns->addWidget(btnRevConfig);
	layConfigBtns->addStretch(4);
	layConfigBtns->addWidget(btnUpdateConfig);
	layConfigBtns->addStretch(2);


	layConfig->addWidget(groupTcp);
	layConfig->addWidget(groupTable);
	layConfig->addLayout(layConfigBtns);


	//初始化配置信息

	le_TcpHost->setText(_tcpHost);
	le_TcpPort->setText(QString::number(_tcpPort));
	le_TcpStatusPort->setText(QString::number(_tcpStatusPort));
	le_TableNum->setText(_tableNum);


	//绑定两按钮
	connect(btnUpdateConfig, &QPushButton::clicked, this, &DialogSettings::slotUpdateBtnClicked);
	connect(btnRevConfig, &QPushButton::clicked, this, &DialogSettings::slotRevBtnClicked);
}

void DialogSettings::slotUpdateBtnClicked() {
	if (le_TcpHost->text().isEmpty() || le_TcpPort->text().isEmpty() || le_TcpStatusPort->text().isEmpty() || le_TableNum->text().isEmpty()) {
		QMessageBox::critical(this, "错误", "关键信息不完整！");
		return;
	}

	//更新信息
	_tcpHost = le_TcpHost->text();
	_tcpPort = le_TcpPort->text().toInt();
	_tcpStatusPort = le_TcpStatusPort->text().toInt();
	_tableNum = le_TableNum->text();

	// 写入文件
	using Json = nlohmann::json;
	Json jsonValue;
	jsonValue["tcpHost"] = _tcpHost.toStdString();
	jsonValue["tcpPort"] = _tcpPort;
	jsonValue["tcpStatusPort"] = _tcpStatusPort;
	jsonValue["tableNum"] = _tableNum.toStdString();

	QFile configFile("config.json");
	if (configFile.open(QFile::WriteOnly)) {
		QByteArray buf = QString::fromStdString(jsonValue.dump(2)).toUtf8();
		configFile.write(buf);
	} else {
		qDebug() << "保存配置时打开配置文件失败";
		QMessageBox::critical(this, "保存失败", "打开配置文件失败");
	}
	configFile.close();

	QMessageBox::information(this, "客户端配置信息更新成功！", "客户端配置信息更新成功，请重启客户端程序！");

	exit(0);
}

void DialogSettings::slotRevBtnClicked() {
	le_TcpHost->setText(_tcpHost);
	le_TcpPort->setText(QString::number(_tcpPort));
	le_TcpStatusPort->setText(QString::number(_tcpStatusPort));
	le_TableNum->setText(_tableNum);
}
