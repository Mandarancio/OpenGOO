#pragma once

#include "QByteArray"
#include "AES.h"

class AesDecrypter
{
public:
    AesDecrypter();

    QByteArray decrypt(const QByteArray& aInData);

private:
    AES _crypt;
};
