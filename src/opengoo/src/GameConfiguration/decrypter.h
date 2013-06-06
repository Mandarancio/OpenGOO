#ifndef DECRYPTER_H
#define DECRYPTER_H

#include "QByteArray"
#include "AES.h"

class Decrypter
{
public:
    Decrypter();

    QByteArray decrypt(QByteArray &indata);

private:    
    AES _crypt;
};

#endif // DECRYPTER_H
