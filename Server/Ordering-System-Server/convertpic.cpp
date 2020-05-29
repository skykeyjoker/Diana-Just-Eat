#include "convertpic.h"

QByteArray toBase64(const QPixmap &pixMap)
{
    QByteArray result;
    QBuffer buf(&result);

    pixMap.save(&buf,"PNG",20);

    result = result.toBase64();
    buf.close();

    //qDebug()<<result;

    return result;
}

QPixmap toQPixmap(const QByteArray &arr)
{
    QByteArray retArr = QByteArray::fromBase64(arr);

    QPixmap result;
    result.loadFromData(retArr);

    return result;
}
