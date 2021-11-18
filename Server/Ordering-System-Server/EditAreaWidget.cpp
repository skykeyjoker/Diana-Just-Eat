#include "EditAreaWidget.h"
EditAreaWidget::EditAreaWidget(QWidget *parent) {
	mainLay = new QVBoxLayout(this);

	QHBoxLayout *lay_typeName = new QHBoxLayout;
	le_typeName = new QLineEdit;
	lay_typeName->addWidget(new QLabel("菜品种类名称："));
	lay_typeName->addWidget(le_typeName);
	lay_typeName->addStretch();
	mainLay->addLayout(lay_typeName);

	QHBoxLayout *lay_typeNum = new QHBoxLayout;
	le_typeNum = new QLineEdit;
	lay_typeNum->addWidget(new QLabel("菜品种类数量："));
	lay_typeNum->addWidget(le_typeNum);
	lay_typeNum->addStretch();
	mainLay->addLayout(lay_typeNum);

	QHBoxLayout *lay_btn = new QHBoxLayout;
	btnOk = new QPushButton("确认");
	btnCancel = new QPushButton("取消");
	lay_btn->addStretch();
	lay_btn->addWidget(btnOk);
	lay_btn->addStretch();
	lay_btn->addWidget(btnCancel);
	lay_btn->addStretch();
	mainLay->addLayout(lay_btn);

	connect(btnOk, &QPushButton::clicked, [&, this]() {
		if (le_typeNum->text().isEmpty() || le_typeName->text().isEmpty()) {
			emit sigMissingContent();
		} else
			emit sigOkBtnClicked();
	});

	connect(btnCancel, &QPushButton::clicked, [&, this]() {
		emit sigCancelBtnClicked();
	});
}

QString EditAreaWidget::getTypeName() const {
	return le_typeName->text();
}

int EditAreaWidget::getTypeNum() const {
	return le_typeNum->text().toInt();
}

void EditAreaWidget::setTypeName(const QString &name) {
	le_typeName->setText(name);
	oldTypeName = name;
}

void EditAreaWidget::setTypeNum(const int num) {
	le_typeNum->setText(QString::number(num));
}

void EditAreaWidget::disableTypeNum() {
	le_typeNum->setReadOnly(true);
}

QString EditAreaWidget::getOldTypeName() const {
	return oldTypeName;
}
