#ifndef DIALOGHISTORYVIEWER_H
#define DIALOGHISTORYVIEWER_H

#include <QWidget>
#include <QDialog>
#include <QIcon>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTextBrowser>
#include <QGroupBox>
#include <QRadioButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDateTimeEdit>
#include <QDateTime>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QDebug>
#include <QDir>

#include "dialogordersviewer.h"
#include "mydatetimeselect.h"

class DialogHistoryViewer : public QDialog
{
    Q_OBJECT
public:
    explicit DialogHistoryViewer(QWidget *parent = nullptr);
    QTableWidget *_table;

    QGroupBox *boxSetting;
    QRadioButton *RadioAll;
    QRadioButton *RadioBefore;
    QRadioButton *RadioBetween;
    //QDateTimeEdit *timeEditFrom;
    //QDateTimeEdit *timeEditTo;
    MyDateTimeSelect *timeEditFrom;
    MyDateTimeSelect *timeEditTo;

    QLabel *lb_count;

    bool connectDb();

    void searchHistory(QString from = "NULL", QString to = "NULL");
signals:

public slots:


private:
    QSqlDatabase sqliteDb;
    int _countAllOrders=0;
    int _countSelectedOrders=0;

};

#endif // DIALOGHISTORYVIEWER_H
