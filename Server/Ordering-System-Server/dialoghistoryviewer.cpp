#include "dialoghistoryviewer.h"

DialogHistoryViewer::DialogHistoryViewer(QWidget *parent) : QDialog(parent) {
	resize(1000, 900);
	setWindowIcon(QIcon(":/Res/at7.ico"));
	setWindowTitle("历史订单");


	//连接sqlite数据库
	bool ret = connectDb();
	if (!ret) {
		QMessageBox::critical(this, "连接错误", "无法连接SQlite数据库");
	}


	//初始化界面
	QVBoxLayout *lay = new QVBoxLayout(this);

	boxSetting = new QGroupBox;
	QHBoxLayout *layBox = new QHBoxLayout(boxSetting);
	RadioAll = new QRadioButton("全部显示");
	RadioBefore = new QRadioButton("显示之前");
	RadioBetween = new QRadioButton("显示之间");
	timeEditFrom = new MyDateTimeSelect;
	timeEditTo = new MyDateTimeSelect;
	QPushButton *btn_search = new QPushButton("查询");

	layBox->addWidget(RadioAll);
	layBox->addStretch(3);
	layBox->addWidget(RadioBefore);
	layBox->addStretch(3);
	layBox->addWidget(RadioBetween);
	layBox->addStretch(3);
	layBox->addWidget(timeEditFrom);
	layBox->addSpacing(20);
	layBox->addWidget(timeEditTo);
	layBox->addStretch(3);
	layBox->addWidget(btn_search);
	layBox->addStretch(3);

	_table = new QTableWidget;

	QHBoxLayout *layBtn = new QHBoxLayout;
	QPushButton *btn_view = new QPushButton("查看该订单");
	QPushButton *btn_chart = new QPushButton("查看销售图表");
	lb_count = new QLabel;

	layBtn->addStretch(4);
	layBtn->addWidget(btn_view);
	layBtn->addStretch(4);
	layBtn->addWidget(btn_chart);
	layBtn->addStretch(4);
	layBtn->addWidget(lb_count);


	lay->addWidget(boxSetting);
	lay->addWidget(_table);
	lay->addLayout(layBtn);


	//订单号 桌号 价格 订单内容 订单备注
	_table->setColumnCount(6);//设置列数
	_table->setHorizontalHeaderLabels(QStringList() << "ID"
													<< "订单号"
													<< "桌号"
													<< "价格"
													<< "订单内容"
													<< "订单备注");//设置水平表头

	//设置列宽
	_table->setColumnWidth(0, 120);
	_table->setColumnWidth(1, 200);
	_table->setColumnWidth(2, 90);
	_table->setColumnWidth(3, 90);
	_table->setColumnWidth(4, 300);
	_table->setColumnWidth(5, 280);

	//让“订单内容”和订单备注随拉伸变化
	_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
	_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);

	//设置table属性
	_table->setFocusPolicy(Qt::NoFocus);                         //失去焦点，让_table_Orders在未选中前不能删除
	_table->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行行为，以行为单位
	_table->setSelectionMode(QAbstractItemView::SingleSelection);//设置选择模式，选择单行
	_table->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置为禁止编辑


	//调整group属性
	RadioAll->setChecked(true);
	timeEditTo->setVisible(false);
	timeEditFrom->setVisible(false);
	btn_search->setVisible(false);


	//单选按钮事件
	connect(RadioAll, &QRadioButton::clicked, [=]() {
		//全部显示
		searchHistory();                //调用参数为空的searchHistory函数
		btn_search->setVisible(false);  //搜索按钮不可视
		timeEditTo->setVisible(false);  //时间截止编辑框不可视
		timeEditFrom->setVisible(false);//时间起始编辑框不可视
	});

	connect(RadioBefore, &QRadioButton::clicked, [=]() {
		//显示之前
		timeEditTo->setVisible(true);   //时间截止编辑框可视
		timeEditFrom->setVisible(false);//时间起始编辑框不可视
		btn_search->setVisible(true);   //搜索按钮可视
	});

	connect(RadioBetween, &QRadioButton::clicked, [=]() {
		//显示之间
		timeEditTo->setVisible(true);  //时间截止编辑框可视
		timeEditFrom->setVisible(true);//时间起始编辑框可视
		btn_search->setVisible(true);  //搜索按钮可视
	});


	//遍历sqlite数据库,插入_table
	boxSetting->setEnabled(false);//先禁止操作

	QSqlQuery query(sqliteDb);
	query.exec("SELECT *FROM Orders");//选中整个表

	while (query.next()) {
		_countAllOrders++;//记录一下总订单数
		//qDebug() << query.value(0).toInt() << query.value(1).toString() << query.value(2).toString()<<query.value(3).toDouble()<<query.value(4).toString()<<query.value(5).toString();
		_table->setRowCount(_table->rowCount() + 1);//插入新行

		int currentRow = _table->rowCount() - 1;//获取当前行
		//赋值当前行
		_table->setItem(currentRow, 0, new QTableWidgetItem(QString::number(query.value(0).toInt())));
		_table->setItem(currentRow, 1, new QTableWidgetItem(query.value(1).toString()));
		_table->setItem(currentRow, 2, new QTableWidgetItem(query.value(2).toString()));
		_table->setItem(currentRow, 3, new QTableWidgetItem(QString::number(query.value(3).toDouble())));
		_table->setItem(currentRow, 4, new QTableWidgetItem(query.value(4).toString()));
		_table->setItem(currentRow, 5, new QTableWidgetItem(query.value(5).toString()));
	}
	boxSetting->setEnabled(true);//遍历完后恢复操作

	//更新信息
	lb_count->setText(tr("共搜索到%1条订单记录").arg(QString::number(_countAllOrders)));


	//绑定按钮
	connect(btn_view, &QPushButton::clicked, [=]() {
		/* 查看订单按钮 */
		if (_table->currentIndex().row() == -1)//判断是否选中一行
		{
			QMessageBox::critical(this, "查看失败", "未选中任何行!");
			return;
		}

		DialogOrdersViewer *dlg = new DialogOrdersViewer;

		dlg->setData(true, _table->item(_table->currentIndex().row(), 1)->text(), _table->item(_table->currentIndex().row(), 2)->text(), _table->item(_table->currentIndex().row(), 3)->text(), _table->item(_table->currentIndex().row(), 4)->text(), _table->item(_table->currentIndex().row(), 5)->text());
		dlg->exec();
	});

	connect(btn_search, &QPushButton::clicked, [=]() {
		/* 搜索按钮 */
		qDebug() << timeEditFrom->dateTime().toString("yyyyMMddhhmmss");
		qDebug() << timeEditTo->dateTime().toString("yyyyMMddhhmmss");

		if ((timeEditFrom->dateTime() > timeEditTo->dateTime()) && !RadioBefore->isChecked())//判断时间起止的合法性
		{
			QMessageBox::critical(this, "错误", "请重新检查时间筛选设置！");
		} else {
			if (timeEditTo->isVisible() && timeEditFrom->isVisible())//如果是“查看之间”
				searchHistory(timeEditFrom->dateTime().toString("yyyyMMddhhmmss"), timeEditTo->dateTime().toString("yyyyMMddhhmmss"));
			if (!timeEditFrom->isVisible())//如果是“查看之前”
				searchHistory("NULL", timeEditTo->dateTime().toString("yyyyMMddhhmmss"));
		}
	});

	connect(btn_chart, &QPushButton::clicked, [=]() {
		DialogChartView dlg;
		dlg.exec();
	});
}

bool DialogHistoryViewer::connectDb() {

	//建立数据连接
	sqliteDb = QSqlDatabase::addDatabase("QSQLITE", "LocalSqlite");
	//设置数据库文件名
	QString dbPath = QDir::currentPath() + "/" + "orders.db";
	qDebug() << dbPath;
	qDebug() << "123";
	sqliteDb.setDatabaseName(dbPath);


	bool ret = sqliteDb.open();

	if (ret) {
		qDebug() << "sqlite数据库连接成功";
	} else
		qDebug() << "sqlite数据库连接失败" << sqliteDb.lastError().text();

	return ret;
}

/* SQLite数据库查询 */
void DialogHistoryViewer::searchHistory(QString from, QString to) {
	boxSetting->setEnabled(false);//遍历时先禁止

	// 格式化时间信息
	QDateTime fromTime = QDateTime::fromString(from, "yyyyMMddhhmmss");
	QDateTime toTime = QDateTime::fromString(to, "yyyyMMddhhmmss");

	qDebug() << fromTime.toString("yyyyMMddhhmmss");
	qDebug() << fromTime.toString("yyyyMMddhhmmss");

	_countSelectedOrders = _countAllOrders;

	if (from == "NULL" && to == "NULL")//如果两个参数都为“NULL”,即搜索全部
	{
		for (int i = 0; i < _table->rowCount(); i++)//将所有的行都取消隐藏
		{
			if (_table->isRowHidden(i) == true) {
				_table->setRowHidden(i, false);
			}
		}
		lb_count->setText(tr("共搜索到%1条订单记录").arg(_countAllOrders));//更新订单信息
	}

	if (from == "NULL" && to != "NULL")//如果为“查看之前”
	{
		for (int i = 0; i < _table->rowCount(); i++) {
			//QDateTime currentTime = QDateTime::fromString(_table->item(i, 1)->text().mid(0, 14), "yyyyMMddhhmmss");//将订单号前半部分即时间取出
			// 提取订单时间部分
			QDateTime currentTime;
			QString currentOrderNum = _table->item(i, 1)->text();
			QString currentOrderTableNum = _table->item(i, 2)->text();
			QString currentTimeStr = currentOrderNum.mid(currentOrderNum.indexOf(currentOrderTableNum) + currentOrderTableNum.length(), -1);
			currentTime = QDateTime::fromString(currentTimeStr, "yyyyMMddhhmmss");
			qDebug() << currentTime.toString("yyyyMMddhhmmss");
			if (currentTime > toTime) {
				qDebug() << "44444";
				_table->setRowHidden(i, true);//隐藏位于设定时间之后的订单
				_countSelectedOrders--;
			} else
				_table->setRowHidden(i, false);//显示合乎条件的订单
		}
		lb_count->setText(tr("共搜索到%1条订单记录").arg(_countSelectedOrders));//更新订单信息
	}

	if (from != "NULL" && to != "NULL")//如果为“查看之间”
	{
		for (int i = 0; i < _table->rowCount(); i++) {
			//QDateTime currentTime = QDateTime::fromString(_table->item(i,1)->text().mid(0,14),"yyyyMMddhhmmss");  //将订单前半部分即时间取出
			// 提取订单时间部分
			QDateTime currentTime;
			QString currentOrderNum = _table->item(i, 1)->text();
			QString currentOrderTableNum = _table->item(i, 2)->text();
			QString currentTimeStr = currentOrderNum.mid(currentOrderNum.indexOf(currentOrderTableNum) + currentOrderTableNum.length(), -1);
			currentTime = QDateTime::fromString(currentTimeStr, "yyyyMMddhhmmss");
			qDebug() << currentTime.toString("yyyyMMddhhmmss");
			if (currentTime < fromTime || currentTime > toTime) {
				qDebug() << "2333333";
				_table->setRowHidden(i, true);//隐藏不位于设定时间段的订单
				_countSelectedOrders--;
			} else
				_table->setRowHidden(i, false);//喜爱你是合乎条件的订单
		}
		lb_count->setText(tr("共搜索到%1条订单记录").arg(_countSelectedOrders));//更新订单信息
	}

	boxSetting->setEnabled(true);//遍历完成后恢复
}
