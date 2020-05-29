#ifndef CONVERTPIC_H
#define CONVERTPIC_H

#include <QtCore>
#include <QBuffer>
#include <QPixmap>
#include <QDebug>

QByteArray toBase64(const QPixmap &pixMap); //QPixmap转Base64

QPixmap toQPixmap(const QByteArray &arr); //Base64转QPixmap

#endif // CONVERTPIC_H
