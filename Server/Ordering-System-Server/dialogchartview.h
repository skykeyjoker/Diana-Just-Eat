#ifndef DIALOGCHARTVIEW_H
#define DIALOGCHARTVIEW_H

#include <QMainWindow>
#include <QtCharts>
#include <QChartView>
#include <QLineSeries>
#include <QPieSeries>
#include <QPieSlice>
#include <QBarSet>
#include <QBarSeries>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QRadioButton>

#include "mydatetimeselect.h"

QT_CHARTS_USE_NAMESPACE

class DialogChartView : public QDialog
{
    Q_OBJECT
public:
    explicit DialogChartView(QWidget *parent = nullptr);

    QVBoxLayout *layMain;
    QTabWidget *tabWidget;

    QVBoxLayout *layYearsTab;
    QChart *years_chart;  //年chart
    QMap<QString, double> *yearsTotal;  //每年对应的总销售量
    QStringList *yearsList;  //年列表
    QBarSeries *years_series;  //多年年销售量条形图series
    QChartView *years_chartView;  //多年年销售量条形图view
    QBarSet *yearsBarSet;

    QVBoxLayout *layMonthsTab;
    QGroupBox *groupBox_months;
    QComboBox *comboBox_months;
    QChart *months_chart;  //月chart
    QMap<QString, QMap<int,double>> *monthsTotal;  //每年每月对应的总销售量
    QBarSeries *months_series;  //月销售量条形图series
    QChartView *months_chartView;  //月销售量条形图view
    QList<QBarSet*> monthsBarSet;

    QVBoxLayout *layDaysTab;
    QGroupBox *groupBox_days;
    MyDateTimeSelect *dateSelector;
    QChart *days_chart;  //日chart
    QMap<QString, QMap<int, QMap<int, double>>> *daysTotal;   //每年每月每日对应的总销售量
    QBarSeries *days_series;
    QChartView *days_chartView;
    QList<QBarSet*> daysBarSet;


    QSqlDatabase sqliteDb;

    void searchMonthsSalesOfYear();
    void searchDaySalesOfMonth();
signals:

};

#endif // DIALOGCHARTVIEW_H
