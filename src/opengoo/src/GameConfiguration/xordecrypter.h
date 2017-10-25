#pragma once

#include "QByteArray"

class XorDecrypter
{
public:
    QByteArray decrypt(const QByteArray& aInData);
};
