#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>

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
