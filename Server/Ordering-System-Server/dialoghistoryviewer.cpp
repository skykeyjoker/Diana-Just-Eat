#include "dialoghistoryviewer.h"

DialogHistoryViewer::DialogHistoryViewer(QWidget *parent) : QDialog(parent)
{
    resize(1000,900);
    setWindowIcon(QIcon(":/Res/at7.ico"));
    setWindowTitle("历史订单");


    QVBoxLayout *lay = new QVBoxLayout(this);


    QGroupBox *boxSetting = new QGroupBox;
    QHBoxLayout *layBox = new QHBoxLayout(boxSetting);
    QRadioButton *RadioAll = new QRadioButton("全部显示");
    QRadioButton *RadioBefore = new QRadioButton("显示之前");
    QRadioButton *RadioBetween = new QRadioButton("显示之间");
    QDateTimeEdit *timeEditFrom = new QDateTimeEdit;
    QDateTimeEdit *timeEditTo = new QDateTimeEdit;

    layBox->addWidget(RadioAll);
    layBox->addStretch(1);
    layBox->addWidget(RadioBefore);
    layBox->addStretch(1);
    layBox->addWidget(RadioBetween);
    layBox->addStretch(3);
    layBox->addWidget(timeEditFrom);
    layBox->addSpacing(20);
    layBox->addWidget(timeEditTo);


    _view = new QTableView;

    lay->addWidget(boxSetting);
    lay->addWidget(_view);
}
