#include "dialogchartview.h"

DialogChartView::DialogChartView(QWidget *parent) : QDialog(parent)
{
    resize(1000,900);
    setWindowIcon(QIcon(":/Res/at7.ico"));
    setWindowTitle("销售图表");


    /* 连接数据库 */
    //建立数据连接
    sqliteDb = QSqlDatabase::addDatabase("QSQLITE","LocalSqlite");
    //设置数据库文件名
    QString dbPath = QDir::currentPath()+"/"+"orders.db";
    qDebug()<<dbPath;
    qDebug()<<"123";
    sqliteDb.setDatabaseName(dbPath);

    bool ret = sqliteDb.open();

    if(!ret)
        qDebug()<<"Can not open the DB";
    else
        qDebug()<<"Open the DB successfully.";


    /* 界面 */
    layMain = new QVBoxLayout(this);
    tabWidget = new QTabWidget;
    layMain->addWidget(tabWidget);

    QWidget *widgetYearsTab = new QWidget(nullptr);
    QWidget *widgetMonthsTab = new QWidget(nullptr);
    QWidget *widgetDaysTab = new QWidget(nullptr);

    tabWidget->addTab(widgetYearsTab,"年销售量");
    tabWidget->addTab(widgetMonthsTab,"月销售量");
    tabWidget->addTab(widgetDaysTab,"日销售量");

    layYearsTab = new QVBoxLayout(widgetYearsTab);
    layMonthsTab = new QVBoxLayout(widgetMonthsTab);
    layDaysTab = new QVBoxLayout(widgetDaysTab);


    /* 实例化对象 */
    yearsList = new QStringList;
    yearsTotal = new QMap<QString, double>;
    years_chart = new QChart;
    years_series  = new QBarSeries;
    years_chartView = new QChartView;
    yearsBarSet = new QBarSet("年份");

    monthsTotal = new QMap<QString, QMap<int, double>>;
    months_chart = new QChart;
    months_series = new QBarSeries;
    months_chartView = new QChartView;
    groupBox_months = new QGroupBox;

    groupBox_days = new QGroupBox;
    dateSelector = new MyDateTimeSelect;
    days_chart = new QChart;
    daysTotal = new QMap<QString, QMap<int, QMap<int, double>>>;
    days_series = new QBarSeries;
    days_chartView = new QChartView;

    /* 数据遍历 */
    QSqlQuery query(sqliteDb);
    query.exec("SELECT *FROM Orders"); //选中整个表

    int count = 0;
    while(query.next())  //遍历表
    {
        QSqlRecord currentRecord = query.record(); //选中一条record

        //2020061618563100001
        QString currentYear = currentRecord.value(1).toString().mid(0,4);  //当记录前年
        double currentSales = currentRecord.value(3).toDouble();  //当前记录总价
        int currentMonth = currentRecord.value(1).toString().mid(4,2).toInt();
        int currentDay = currentRecord.value(1).toString().mid(6,2).toInt();
        QString currentMenusList = currentRecord.value(4).toString();  //当前记录的订单内容


        qDebug()<<"Current year:"<<currentYear<<" "<<"Current month:"<<currentMonth<<" "<<"Current day:"<<currentDay;

        //年
        if(!yearsList->contains(currentYear)) //如果不存在这一年
        {
            yearsList->append(currentYear); //插入到年列表
            yearsTotal->insert(currentYear,currentSales);  //插入年销售量map

            //把该年的每月销售量初始化一下
            QMap<int ,double> currentYearMonthSales;
            qDebug()<<currentYear<<"has not been added yet";
            for(int i=1; i<=12; ++i)
            {
                qDebug()<<"Insert month:"<<i;
                currentYearMonthSales.insert(i,0);

                //把每月每日也初始化
                QMap<int,QMap<int, double>> monthsMap = daysTotal->value(currentYear);  //month
                QMap<int, double> daysMap; //day
                for(int j=1; j<=31; j++)
                {
                    daysMap.insert(j,0);  //插入31天的数据
                }
                monthsMap.insert(i,daysMap); //插入12个月的数据
                daysTotal->insert(currentYear,monthsMap);
            }
            qDebug()<<"currentYearMonthSales' size:"<<currentYearMonthSales.size();
            monthsTotal->insert(currentYear,currentYearMonthSales);
        }
        else //存在这一年
        {
            yearsTotal->insert(currentYear,yearsTotal->value(currentYear)+currentSales);
        }

        //月
        QMap<int ,double> currentYearMonthSales = monthsTotal->value(currentYear);
        currentYearMonthSales.insert(currentMonth,monthsTotal->value(currentYear).value(currentMonth)+currentSales);
        monthsTotal->insert(currentYear,currentYearMonthSales);

        //日
        QMap<int, double> currentYearMonthDaySales = daysTotal->value(currentYear).value(currentMonth);
        currentYearMonthDaySales.insert(currentDay,currentYearMonthDaySales.value(currentDay)+currentSales);
        QMap<int, QMap<int, double>> currentYearMonthSales_day = daysTotal->value(currentYear);
        currentYearMonthSales_day.insert(currentMonth, currentYearMonthDaySales);
        daysTotal->insert(currentYear,currentYearMonthSales_day);

        count++;
    }
    qDebug()<<count;


    /* 多年年销售量条形图Tab */
    //导入barset数据
    QMap<QString, double>::const_iterator ite_year;
    for(ite_year=yearsTotal->constBegin();ite_year!=yearsTotal->constEnd();++ite_year)
    {
        qDebug()<<"ite_year.value()="<<ite_year.value();
        *yearsBarSet<<ite_year.value();
    }
    yearsBarSet->setLabelColor(QColor(0,0,255)); //设置条形数据颜色

    years_series->append(yearsBarSet); //添加barset
    years_series->setVisible(true);
    years_series->setLabelsVisible(true);

    years_chart->addSeries(years_series);  //添加series
    years_chart->setTheme(QChart::ChartThemeLight); //设置主题
    years_chart->setDropShadowEnabled(true);  //开启背景阴影
    years_chart->setTitle("多年年销售量条形图");  //设置标题
    years_chart->setTitleFont(QFont("思源黑体"));  //设置标题字体
    years_chart->setTitleBrush(QColor(0,0,255));  //设置标题颜色

    //创建坐标轴
    QBarCategoryAxis *axisX = new QBarCategoryAxis;  //X轴
    for(int i=0; i<yearsList->size(); ++i)
    {
        axisX->append(yearsList->at(i));
    }
    axisX->setLabelsColor(QColor(7,28,96));  //设置坐标轴字体颜色

    QValueAxis *axisY = new QValueAxis;  //Y轴
    axisY->setRange(0,0);
    axisY->setTitleText("总销售量/元");

    years_chart->setAxisX(axisX,years_series);
    years_chart->setAxisY(axisY,years_series);

    //修改说明样式
    years_chart->legend()->setVisible(true);
    years_chart->legend()->setAlignment(Qt::AlignBottom);//底部对齐
    years_chart->legend()->setBackgroundVisible(true);//设置背景是否可视
    years_chart->legend()->setAutoFillBackground(true);//设置背景自动填充
    years_chart->legend()->setColor(QColor(222,233,251));//设置颜色
    years_chart->legend()->setLabelColor(QColor(0,100,255));//设置标签颜色
    years_chart->legend()->setMaximumHeight(50);

    years_chartView->setChart(years_chart);
    years_chartView->setRenderHint(QPainter::Antialiasing);

    layYearsTab->addWidget(years_chartView);



    /* 月销售量条形图 */
    layMonthsTab->addWidget(groupBox_months);
    QHBoxLayout *layComboBox = new QHBoxLayout(groupBox_months);
    comboBox_months = new QComboBox;
    QPushButton *btnSearch = new QPushButton("查询");

    layComboBox->addWidget(comboBox_months);
    layComboBox->addSpacing(100);
    layComboBox->addWidget(btnSearch);


    comboBox_months->addItem(tr("全部年份:%1-%2").arg(yearsList->first()).arg(yearsList->last()));
    connect(btnSearch,&QPushButton::clicked,this,&DialogChartView::searchMonthsSalesOfYear);

    //遍历每一年
    QMap<QString, QMap<int, double>>::const_iterator iteMonth;
    for(iteMonth=monthsTotal->constBegin();iteMonth!=monthsTotal->constEnd();++iteMonth)
    {
        QMap<int ,double>::const_iterator j;
        QMap<int ,double>current = iteMonth.value();  //取出当前年对应的QMap
        //qDebug()<<iteMonth.value().size();
        //qDebug()<< current.size();
        QBarSet *currentBar = new QBarSet(tr("%1年").arg(iteMonth.key()));  //创建一个当前年的QBarSet
        comboBox_months->addItem(iteMonth.key());  //添加当前年到QComboBox

        for(j=current.constBegin();j!=current.constEnd();++j)  //遍历当前年的12个月
        {
            //qDebug()<<j.key()<<" "<<j.value();
            *currentBar<<j.value();
        }
        currentBar->setLabelColor(QColor(0,0,255));
        monthsBarSet.append(currentBar);

        months_series->append(currentBar); //添加barset
    }

    months_series->setVisible(true);
    months_series->setLabelsVisible(true);

    months_chart->addSeries(months_series);  //添加series
    months_chart->setTheme(QChart::ChartThemeLight); //设置主题
    months_chart->setDropShadowEnabled(true);  //开启背景阴影
    months_chart->setTitle("月销售量条形图");  //设置标题
    months_chart->setTitleFont(QFont("思源黑体"));  //设置标题字体
    months_chart->setTitleBrush(QColor(0,0,255));  //设置标题颜色

    //创建坐标轴
    QBarCategoryAxis *axisX_Month = new QBarCategoryAxis;  //X轴
    for(int i=0; i<12; ++i)
    {
        axisX_Month->append(QString::number(i+1));
    }
    axisX_Month->setLabelsColor(QColor(7,28,96));  //设置坐标轴字体颜色

    QValueAxis *axisY_Month = new QValueAxis;  //Y轴
    axisY_Month->setRange(0,0);
    axisY_Month->setTitleText("总销售量/元");

    months_chart->setAxisX(axisX_Month,months_series);
    months_chart->setAxisY(axisY_Month,months_series);

    //修改说明样式
    months_chart->legend()->setVisible(true);
    months_chart->legend()->setAlignment(Qt::AlignBottom);//底部对齐
    months_chart->legend()->setBackgroundVisible(true);//设置背景是否可视
    months_chart->legend()->setAutoFillBackground(true);//设置背景自动填充
    months_chart->legend()->setColor(QColor(222,233,251));//设置颜色
    months_chart->legend()->setLabelColor(QColor(0,100,255));//设置标签颜色
    months_chart->legend()->setMaximumHeight(50);

    months_chartView->setChart(months_chart);
    months_chartView->setRenderHint(QPainter::Antialiasing);

    layMonthsTab->addWidget(months_chartView);


    /* 日销售量条形图 */
    layDaysTab->addWidget(groupBox_days);
    QHBoxLayout *layDateSelector = new QHBoxLayout(groupBox_days);
    QPushButton *btnSearch_date = new QPushButton("查询");

    layDateSelector->addWidget(dateSelector);
    layDateSelector->addSpacing(100);
    layDateSelector->addWidget(btnSearch_date);

    //设置日期选择框范围
    dateSelector->calendar->setDateRange(QDate(yearsList->first().toInt(),1,1), QDate(yearsList->last().toInt(),12,31));
    dateSelector->edit->setDate(QDate(yearsList->first().toInt(),1,1));

    connect(btnSearch_date,&QPushButton::clicked,this,&DialogChartView::searchDaySalesOfMonth);

    //遍历每一年每一月
    QMap<QString, QMap<int, QMap<int, double>>>::const_iterator i;
    for(i=daysTotal->constBegin(); i!=daysTotal->constEnd(); ++i)
    {
        QMap<int, QMap<int, double>>current = i.value();
        QMap<int, QMap<int, double>>::const_iterator j;
        for(j=current.constBegin(); j!=current.constEnd(); ++j)
        {
            QBarSet *currentBar = new QBarSet(tr("%1年%2月").arg(i.key()).arg(j.key()));

            QMap<int, double> currentDay = j.value();
            QMap<int, double>::const_iterator k;
            for(k=currentDay.constBegin(); k!=currentDay.constEnd(); ++k)
            {
                //qDebug()<<k.key()<<" "<<k.value();
                *currentBar<<k.value();
            }

            currentBar->setLabelColor(QColor(0,0,255));
            daysBarSet.append(currentBar);

            //days_series->append(currentBar);  //添加barset
        }
    }

    days_series->append(daysBarSet.at(0));

    days_series->setVisible(true);
    days_series->setLabelsVisible(true);

    days_chart->addSeries(days_series);  //添加series
    days_chart->setTheme(QChart::ChartThemeLight); //设置主题
    days_chart->setDropShadowEnabled(true);  //开启背景阴影
    days_chart->setTitle("日销售量条形图");  //设置标题
    days_chart->setTitleFont(QFont("思源黑体"));  //设置标题字体
    days_chart->setTitleBrush(QColor(0,0,255));  //设置标题颜色

    //创建坐标轴
    QBarCategoryAxis *axisX_Day = new QBarCategoryAxis;  //X轴
    for(int i=0; i<31; ++i)
    {
        axisX_Day->append(QString::number(i+1));
    }
    axisX_Day->setLabelsColor(QColor(7,28,96));  //设置坐标轴字体颜色

    QValueAxis *axisY_Day = new QValueAxis;  //Y轴
    axisY_Day->setRange(0,0);
    axisY_Day->setTitleText("总销售量/元");

    days_chart->setAxisX(axisX_Day,days_series);
    days_chart->setAxisY(axisY_Day,days_series);

    //修改说明样式
    days_chart->legend()->setVisible(true);
    days_chart->legend()->setAlignment(Qt::AlignBottom);//底部对齐
    days_chart->legend()->setBackgroundVisible(true);//设置背景是否可视
    days_chart->legend()->setAutoFillBackground(true);//设置背景自动填充
    days_chart->legend()->setColor(QColor(222,233,251));//设置颜色
    days_chart->legend()->setLabelColor(QColor(0,100,255));//设置标签颜色
    days_chart->legend()->setMaximumHeight(50);

    days_chartView->setChart(days_chart);
    days_chartView->setRenderHint(QPainter::Antialiasing);

    layDaysTab->addWidget(days_chartView);

}

void DialogChartView::searchMonthsSalesOfYear()
{
    months_series = new QBarSeries;
    months_chart->removeSeries(months_chart->series().at(0));  //先删除当前的series

    if(comboBox_months->currentIndex()==0)
    {
        qDebug()<<"All selected";
        months_series->append(monthsBarSet);
    }
    else
    {
        qDebug()<<comboBox_months->currentText()<<" "<<comboBox_months->currentIndex()<<" selected";
        months_series->append(monthsBarSet.at(comboBox_months->currentIndex()-1));
    }

    months_series->setLabelsVisible(true);
    months_chart->addSeries(months_series);
}

void DialogChartView::searchDaySalesOfMonth()
{
    QString str = dateSelector->edit->text().section('/',0,1);
    QString strYear = str.section('/',0,0);
    QString strMonth = str.section('/',1,1);

    /*
     * 2016
     * 1 2 3 4 5 6 7 8 9 10 11 12
     * 0 1 2 3 4 5 6 7 8 9 10 11
     * 2017
     * 1 2 3 4 5 6 7 8 9 10 11 12
     * 12 13 14 15 16 17 18 19 20 21 22 23
     * 2018
     * 1 2 3 4 5 6 7 8 9 10 11 12
     * 24 25 26 27 28 29 30 31 32 33 34 35
    */
    int index = 0;
    qDebug()<<(yearsList->last().toInt()-yearsList->first().toInt())*12 + 12 - 1;

    index = (strYear.toInt()-yearsList->first().toInt())*12 + strMonth.toInt() - 1;

    if(index > (yearsList->last().toInt()-yearsList->first().toInt())*12 + 12 - 1)
    {
        QMessageBox::critical(this,"日期选择错误","无该月订单!");
        return;
    }

    days_series = new QBarSeries;
    days_series->append(daysBarSet.at(index));
    days_chart->removeSeries(days_chart->series().at(0));  //先删除当前的series
    days_series->setLabelsVisible(true);
    days_chart->addSeries(days_series);
}
