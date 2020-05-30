#include "timestamp.h"

QString getTimeStamp()
{
    QDateTime dateTime(QDateTime::currentDateTime());
    QString str = dateTime.toString("yyyyMMddhhmmss");

    return str;
}
