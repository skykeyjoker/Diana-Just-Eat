#include "clientmainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientMainWindow w;
    w.show();
    return a.exec();
}
