#ifndef ORDERING_SYSTEM_DIALOGEDITMENUTYPE_H
#define ORDERING_SYSTEM_DIALOGEDITMENUTYPE_H

#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "EditAreaWidget.h"

class DialogEditMenuType : public QDialog {
	Q_OBJECT
public:
	explicit DialogEditMenuType(const QHash<QString, int>& menuTypeNumHash, QWidget* parent = nullptr);
	~DialogEditMenuType();

private slots:
	void slotBtnAddClicked();
	void slotBtnEditClicked();
	void slotBtnDelClicked();

public slots:
	void closeEvent(QCloseEvent*);//重写退出事件

signals:
	void sigAddNewType(const QString typeName, const int typeNum);
	void sigEditType(const QString oldTypeName, const QString newTypeName, const int typeNum);
	void sigDelType(const QString typeName);

private:
	QVBoxLayout* mainLay;

	QTableWidget* _table;

	QPushButton* btnAdd;
	QPushButton* btnEdit;
	QPushButton* btnDel;

	EditAreaWidget* addArea;
	EditAreaWidget* editArea;

private:
	QHash<QString, int> m_menuTypeNumHash;
	int currentRowIndex;
};


#endif//ORDERING_SYSTEM_DIALOGEDITMENUTYPE_H
