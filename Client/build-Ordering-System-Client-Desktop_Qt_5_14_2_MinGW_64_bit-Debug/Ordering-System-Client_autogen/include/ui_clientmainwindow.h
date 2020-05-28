/********************************************************************************
** Form generated from reading UI file 'clientmainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENTMAINWINDOW_H
#define UI_CLIENTMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientMainWindow
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ClientMainWindow)
    {
        if (ClientMainWindow->objectName().isEmpty())
            ClientMainWindow->setObjectName(QString::fromUtf8("ClientMainWindow"));
        ClientMainWindow->resize(800, 600);
        centralwidget = new QWidget(ClientMainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        ClientMainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ClientMainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        ClientMainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ClientMainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ClientMainWindow->setStatusBar(statusbar);

        retranslateUi(ClientMainWindow);

        QMetaObject::connectSlotsByName(ClientMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ClientMainWindow)
    {
        ClientMainWindow->setWindowTitle(QCoreApplication::translate("ClientMainWindow", "ClientMainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClientMainWindow: public Ui_ClientMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTMAINWINDOW_H
