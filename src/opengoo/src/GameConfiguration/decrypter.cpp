#include "decrypter.h"

Decrypter::Decrypter()
{
    QByteArray wogkey = QByteArray::fromHex(
                "0D0607070C01080506090904060D030F03060E010E02070B");
    _crypt.SetParameters(wogkey.size() * 8);
    unsigned char* key = reinterpret_cast<unsigned char*>(wogkey.data());
    _crypt.StartDecryption(key);
}

QByteArray Decrypter::decrypt(QByteArray &indata)
{        
    int size = indata.size();
    QByteArray outdata(size, 0x0);

    unsigned char* datain = reinterpret_cast<unsigned char*>(indata.data());
    unsigned char* dataout = reinterpret_cast<unsigned char*>(outdata.data());
    unsigned long numBlocks = size / 16;

    _crypt.Decrypt(datain, dataout, numBlocks);

    int i = outdata.indexOf(QByteArray::fromHex("FD").data());
    outdata.remove(i, size);

    return outdata;
}
