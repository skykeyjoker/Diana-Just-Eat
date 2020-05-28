#ifndef DIALOGADDRECORD_H
#define DIALOGADDRECORD_H


#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTextBrowser>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QPixmap>


class DialogAddRecord : public QDialog
{
    Q_OBJECT
public:
    explicit DialogAddRecord(QWidget *parent = nullptr);

    // dish info
    QString _dishName;
    QString _dishType;
    QString _dishInfo;
    QString _dishPrice;
    QByteArray _dishPhoto;

    QString picPath;

signals:

};

#endif // DIALOGADDRECORD_H
