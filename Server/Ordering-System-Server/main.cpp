#include "servermainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ServerMainWindow w;
    w.show();

    return a.exec();
}
