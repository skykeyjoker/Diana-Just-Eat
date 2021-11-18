#ifndef ORDERING_SYSTEM_EDITAREAWIDGET_H
#define ORDERING_SYSTEM_EDITAREAWIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class EditAreaWidget : public QWidget {
	Q_OBJECT
public:
	explicit EditAreaWidget(QWidget* parent = nullptr);

public:
	QString getTypeName() const;
	QString getOldTypeName() const;
	int getTypeNum() const;

	void setTypeName(const QString& name);
	void setTypeNum(const int num);

	void disableTypeNum();

signals:
	void sigOkBtnClicked();
	void sigCancelBtnClicked();
	void sigMissingContent();

private:
	QVBoxLayout* mainLay;
	QLineEdit* le_typeName;
	QLineEdit* le_typeNum;
	QPushButton* btnOk;
	QPushButton* btnCancel;

private:
	QString oldTypeName;
};


#endif//ORDERING_SYSTEM_EDITAREAWIDGET_H
