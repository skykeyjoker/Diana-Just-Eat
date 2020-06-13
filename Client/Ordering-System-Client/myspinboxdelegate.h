#ifndef MYSPINBOXDELEGATE_H
#define MYSPINBOXDELEGATE_H

#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QWidget>
#include <QModelIndex>
#include <QObject>
#include <QPainter>
#include <QBrush>
#include <QPalette>

#include "myspinbox.h"

class MySpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    MySpinBoxDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

signals:

};

#endif // MYSPINBOXDELEGATE_H
