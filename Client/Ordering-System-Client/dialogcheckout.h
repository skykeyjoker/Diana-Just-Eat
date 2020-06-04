#ifndef DIALOGCHECKOUT_H
#define DIALOGCHECKOUT_H

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include "cartitem.h"

class DialogCheckOut : public QDialog
{
    Q_OBJECT
public:
    explicit DialogCheckOut(const QList<CartItem>&cartlist, int totalNum, int totalPrice, QWidget *parent = nullptr);
    QTextEdit *_textNote;
signals:
    void signalReadyCheckOut(QString note);

};

#endif // DIALOGCHECKOUT_H
