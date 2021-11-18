#include "DialogEditMenuType.h"

DialogEditMenuType::DialogEditMenuType(const QHash<QString, int> &menuTypeNumHash, QWidget *parent)
	: m_menuTypeNumHash(menuTypeNumHash),
	  QDialog(parent) {
	setWindowTitle("菜品种类编辑");
	setWindowIcon(QIcon(":/Res/at8.ico"));


	mainLay = new QVBoxLayout(this);

	_table = new QTableWidget;
	mainLay->addWidget(_table);

	//设置_table属性
	_table->setColumnCount(2);//列表仅需两列
	_table->setHorizontalHeaderLabels(QStringList() << "菜品种类"
													<< "数量");//设置列表头
	_table->setColumnWidth(0, 230);
	_table->setColumnWidth(1, 100);
	_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);//让“菜品”列随界面缩放而拉伸
	_table->setSelectionBehavior(QAbstractItemView::SelectRows);              //设置选择行行为，以行为单位
	_table->setSelectionMode(QAbstractItemView::SingleSelection);             //设置选择模式，选择单行
	_table->setEditTriggers(QAbstractItemView::NoEditTriggers);               //设置为禁止编辑

	QHBoxLayout *btnLay = new QHBoxLayout;
	btnAdd = new QPushButton("添加菜品种类");
	btnEdit = new QPushButton("编辑菜品种类");
	btnDel = new QPushButton("删除菜品种类");

	btnLay->addStretch();
	btnLay->addWidget(btnAdd);
	btnLay->addStretch();
	btnLay->addWidget(btnEdit);
	btnLay->addStretch();
	btnLay->addWidget(btnDel);
	btnLay->addStretch();
	mainLay->addLayout(btnLay);

	connect(btnAdd, &QPushButton::clicked, this, &DialogEditMenuType::slotBtnAddClicked);
	connect(btnEdit, &QPushButton::clicked, this, &DialogEditMenuType::slotBtnEditClicked);
	connect(btnDel, &QPushButton::clicked, this, &DialogEditMenuType::slotBtnDelClicked);

	editArea = new EditAreaWidget;
	addArea = new EditAreaWidget;
	mainLay->addWidget(editArea);
	mainLay->addWidget(addArea);

	//editArea->disableTypeNum();

	connect(addArea, &EditAreaWidget::sigOkBtnClicked, [=]() {
		addArea->hide();
		// Add Row
		_table->insertRow(_table->rowCount());
		_table->setItem(_table->rowCount() - 1, 0, new QTableWidgetItem(addArea->getTypeName()));
		_table->setItem(_table->rowCount() - 1, 1, new QTableWidgetItem(QString::number(addArea->getTypeNum())));
		emit sigAddNewType(addArea->getTypeName(), addArea->getTypeNum());// 发送种类添加信号
		QMessageBox::information(this, "编辑成功", "成功添加菜品种类：" + addArea->getTypeName());
	});
	connect(addArea, &EditAreaWidget::sigCancelBtnClicked, [=]() {
		addArea->hide();
	});
	connect(addArea, &EditAreaWidget::sigMissingContent, [=]() {
		QMessageBox::critical(this, "添加失败", "请填写菜品种类名称及数量！");
	});

	connect(editArea, &EditAreaWidget::sigOkBtnClicked, [=]() {
		editArea->hide();
		// Update Row
		_table->setItem(currentRowIndex, 0, new QTableWidgetItem(editArea->getTypeName()));
		_table->setItem(currentRowIndex, 1, new QTableWidgetItem(QString::number(editArea->getTypeNum())));
		emit sigEditType(editArea->getOldTypeName(), editArea->getTypeName(), editArea->getTypeNum());// 发送种类修改信号
		QMessageBox::information(this, "编辑成功", "成功修改菜品种类：" + editArea->getTypeName());
	});
	connect(editArea, &EditAreaWidget::sigCancelBtnClicked, [=]() {
		editArea->hide();
	});
	connect(editArea, &EditAreaWidget::sigMissingContent, [=]() {
		QMessageBox::critical(this, "添加失败", "请填写菜品种类名称及数量！");
	});

	// 加载时先隐藏
	editArea->hide();
	addArea->hide();

	// 加载菜品种类
	qDebug() << m_menuTypeNumHash.size();
	for (const auto &currentType : m_menuTypeNumHash.keys()) {
		int currentTypeNum = m_menuTypeNumHash[currentType];
		qDebug() << currentType << currentTypeNum;

		_table->insertRow(_table->rowCount());
		int currentRowIndex = _table->rowCount() - 1;

		_table->setItem(currentRowIndex, 0, new QTableWidgetItem(currentType));
		_table->setItem(currentRowIndex, 1, new QTableWidgetItem(QString::number(currentTypeNum)));
	}
}

DialogEditMenuType::~DialogEditMenuType() {
}

void DialogEditMenuType::closeEvent(QCloseEvent *event) {
	int ret = QMessageBox::question(this, "确认取消", "您确认要退出菜品种类编辑吗？");

	if (ret == QMessageBox::Yes) {
		event->accept();
	} else
		event->ignore();
}

void DialogEditMenuType::slotBtnAddClicked() {
	addArea->setTypeName("");
	addArea->setTypeNum(0);
	if (addArea->isHidden()) {
		addArea->show();
	}
	if (!editArea->isHidden()) {
		editArea->hide();
	}
}

void DialogEditMenuType::slotBtnEditClicked() {
	if (_table->currentRow() == -1) {
		QMessageBox::critical(this, "编辑失败", "请先选择要编辑的菜品种类！");
		return;
	}
	currentRowIndex = _table->currentRow();
	QString currentTypeName = _table->item(_table->currentRow(), 0)->text();
	int currentTypeNum = _table->item(_table->currentRow(), 1)->text().toInt();

	editArea->setTypeName(currentTypeName);
	editArea->setTypeNum(currentTypeNum);
	if (!addArea->isHidden()) {
		addArea->hide();
	}
	if (editArea->isHidden()) {
		editArea->show();
	}
}

void DialogEditMenuType::slotBtnDelClicked() {
	if (_table->currentRow() == -1) {
		QMessageBox::critical(this, "删除失败", "请先选择要删除的菜品种类！");
		return;
	}

	currentRowIndex = _table->currentRow();
	QString currentTypeName = _table->item(_table->currentRow(), 0)->text();
	int ret = QMessageBox::question(this, "删除确认", tr("确认要删除%1吗？").arg(currentTypeName));
	if (ret == QMessageBox::Yes) {
		_table->removeRow(currentRowIndex);
		emit sigDelType(currentTypeName);
	}
}
