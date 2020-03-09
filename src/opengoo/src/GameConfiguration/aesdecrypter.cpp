#include "aesdecrypter.h"
#include "../../../extlibs/AES/AES.cpp"

AesDecrypter::AesDecrypter()
{
    static const uchar key[] =
    {
        0x0D, 0x06, 0x07, 0x07, 0x0C, 0x01, 0x08, 0x05,
        0x06, 0x09, 0x09, 0x04, 0x06, 0x0D, 0x03, 0x0F,
        0x03, 0x06, 0x0E, 0x01, 0x0E, 0x02, 0x07, 0x0B
    };

    _crypt.SetParameters(196);
    _crypt.StartDecryption(key);
}

QByteArray AesDecrypter::decrypt(const QByteArray& aInData)
{        
    int size = aInData.size();
    QByteArray outData(size, 0x0);
    unsigned long numBlocks = size / 16;

    _crypt.Decrypt(reinterpret_cast<const uchar*>(aInData.data()), reinterpret_cast<uchar*>(outData.data()), numBlocks);

    int i = outData.indexOf(0xFD);
    outData.remove(i, size);

    return outData;
}
