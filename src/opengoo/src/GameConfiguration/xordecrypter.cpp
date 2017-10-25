#include "xordecrypter.h"

QByteArray XorDecrypter::decrypt(const QByteArray& aInData)
{
    int size = aInData.size();
    QByteArray outData(size, 0x0);
    int salt = (((size & 1) << 6) | ((size & 2) << 3) | (size & 4)) ^ 0xab;

    for (int i = 0; i < size; ++i)
    {
        outData[i] = (salt ^ aInData[i]);
        salt = ((salt & 0x7f) << 1 | (salt & 0x80) >> 7) ^ aInData[i];
    }

    return outData;
}
