#include "dialogeditrecord.h"

DialogEditRecord::DialogEditRecord(QWidget *parent) : QDialog(parent) {
	setFixedSize(1000, 800);
	setWindowTitle("修改菜品信息");
	setWindowIcon(QIcon(":/Res/at8.ico"));

	//初始化界面
	QVBoxLayout *vlay = new QVBoxLayout(this);

	QHBoxLayout *hlay_Name = new QHBoxLayout(NULL);
	QLabel *lb_Name = new QLabel("菜品名称：");
	le_Name = new QLineEdit;
	hlay_Name->addWidget(lb_Name);
	hlay_Name->addWidget(le_Name);

	QHBoxLayout *hlay_Type = new QHBoxLayout(NULL);
	QLabel *lb_Type = new QLabel("菜品种类：");
	le_Type = new QLineEdit;
	hlay_Type->addWidget(lb_Type);
	hlay_Type->addWidget(le_Type);

	QHBoxLayout *hlay_Info = new QHBoxLayout(NULL);
	QLabel *lb_Info = new QLabel("菜品描述：");
	le_Info = new QTextEdit;
	hlay_Info->addWidget(lb_Info);
	hlay_Info->addWidget(le_Info);

	QHBoxLayout *hlay_Price = new QHBoxLayout(NULL);
	QLabel *lb_Price = new QLabel("菜品价格：");
	le_Price = new QLineEdit;
	hlay_Price->addWidget(lb_Price);
	hlay_Price->addWidget(le_Price);

	QHBoxLayout *hlay_Photo = new QHBoxLayout(NULL);
	QLabel *lb_Photo = new QLabel("菜品图片：");
	le_Photo = new QLineEdit;
	le_Photo->setMaximumSize(600, 400);
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
	brower = new QLabel;
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


	//连接选择图片按钮信号与槽
	connect(btn_Photo, &QPushButton::clicked, [=]() {
		picPath = QFileDialog::getOpenFileName(this, "选择菜品图片", "", "图片文件 (*.png *.jpg *.jpeg)");//打开文件对话框
		if (picPath.isEmpty())                                                                            //如果未打开任何文件
		{
			QMessageBox::critical(this, "错误", "打开文件失败");
		} else {
			le_Photo->setText(picPath);

			//预览图片
			_pic = QPixmap(picPath);
			brower->setPixmap(_pic);
		}
	});


	//取消按钮
	connect(btnCancel, &QPushButton::clicked, this, &DialogEditRecord::slotBtnCancelClicked);
	//提交按钮
	connect(btnSubmit, &QPushButton::clicked, this, &DialogEditRecord::slotBtnSubmitClicked);
}

void DialogEditRecord::setValue(int dishId, const QString &dishName, const QString &dishType, const QString &dishInfo, const double dishPrice, const QString &dishPhoto) {
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
	le_Price->setText(QString::number(_dishPrice));

	//显示图片图片
	brower->setPixmap(QPixmap(QDir::currentPath() + "/Pic/" + dishPhoto));
}

void DialogEditRecord::slotBtnCancelClicked() {
	close();
}

void DialogEditRecord::slotBtnSubmitClicked() {
	bool photoUpdated = false;

	if (le_Info->toPlainText().isEmpty() || le_Name->text().isEmpty() || le_Type->text().isEmpty() || le_Price->text().isEmpty()) {
		QMessageBox::critical(this, "提交失败", "关键信息不完整，请填写必要信息后再提交！");
		return;
	}

	//赋值最新信息
	_dishName = le_Name->text();
	_dishType = le_Type->text();
	_dishInfo = le_Info->toPlainText();
	_dishPrice = le_Price->text().toDouble();

	// 更新图片
	if (!le_Photo->text().isEmpty())//如果图片路径编辑框不为空，那就是选择了新的图片
	{
		QString newPhotoFileName = le_Photo->text().mid(le_Photo->text().lastIndexOf("/") + 1, -1);

		// 删除旧图片
		QDir::current().remove("/Pic/" + _dishPhoto);

		_dishPhoto = newPhotoFileName;
		qDebug() << _dishPhoto;
		qDebug() << "图片更新";
		photoUpdated = true;

		//拷贝图片到程序运行目录
		QString newPath = QDir::currentPath() + "/Pic/" + newPhotoFileName;
		qDebug() << "newPath" << newPath;
		QFileInfo info(newPath);
		QString fileName = info.fileName();
		//判断文件是否存在
		if (info.exists()) {
			qDebug() << "文件已存在";
			QDir dir(QDir::currentPath() + "/Pic");
			qDebug() << "dir:" << dir.dirName();
			qDebug() << dir.remove(fileName);//删除之前的缓存文件
		}
		if (QFile::copy(picPath, newPath)) {
			qDebug() << "拷贝成功";
		} else
			qDebug() << "拷贝失败";
	}

	emit signalUpdate(_dishId, _dishName, _dishType, _dishInfo, _dishPrice, _dishPhoto, photoUpdated);//发送菜品信息更新信号
	QMessageBox::information(this, "成功", "修改菜品信息成功！");
}

//重写退出事件
void DialogEditRecord::closeEvent(QCloseEvent *event) {
	int ret = QMessageBox::question(this, "确认取消", "您确认要退出添加菜品吗？");

	if (ret == QMessageBox::Yes) {
		event->accept();
	} else
		event->ignore();
}
