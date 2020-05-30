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
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDateTimeEdit>
#include <QDateTime>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QPixmap>
#include <QCloseEvent>


class DialogHistoryViewer : public QDialog
{
    Q_OBJECT
public:
    explicit DialogHistoryViewer(QWidget *parent = nullptr);
    QTableView *_view;
signals:

};

#endif // DIALOGHISTORYVIEWER_H
