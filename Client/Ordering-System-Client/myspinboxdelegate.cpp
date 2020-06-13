#include "myspinboxdelegate.h"

MySpinBoxDelegate::MySpinBoxDelegate(QObject *parent) : QItemDelegate(parent)
{

}

void MySpinBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);
}

QSize MySpinBoxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QItemDelegate::sizeHint(option, index);
}

QWidget *MySpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    MySpinBox *spinBox = new MySpinBox(parent);
    //spinBox->installEventFilter(const_cast<MySpinBoxDelegate*>(this));

    return spinBox;
}

void MySpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString data = index.model()->data(index,Qt::DisplayRole).toString();
    MySpinBox *spinBox = qobject_cast<MySpinBox*>(editor);

    spinBox->setText(data);
}

void MySpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    MySpinBox *spinBox = qobject_cast<MySpinBox*>(editor);
    model->setData(index,spinBox->text());
}



