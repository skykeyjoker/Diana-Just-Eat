#include "clientmainwindow.h"
#include "./ui_clientmainwindow.h"

ClientMainWindow::ClientMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientMainWindow)
{
    ui->setupUi(this);
}

ClientMainWindow::~ClientMainWindow()
{
    delete ui;
}

