#include "EncryptDecrypt.h"

QString XorEncryptDecrypt(const QString &str, const char &key)
{
    QString result;
    QByteArray arr = str.toUtf8();


    for(int i=0; i<arr.size(); i++)
    {
        arr[i] = arr[i] ^ key;
    }

    result = QString::fromUtf8(arr);

    return result;
}
