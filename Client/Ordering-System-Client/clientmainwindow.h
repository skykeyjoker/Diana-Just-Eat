#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>

#include "tcpclient.h"


QT_BEGIN_NAMESPACE
namespace Ui { class ClientMainWindow; }
QT_END_NAMESPACE

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientMainWindow(QWidget *parent = nullptr);
    ~ClientMainWindow();

private:
    Ui::ClientMainWindow *ui;
};
#endif // CLIENTMAINWINDOW_H
