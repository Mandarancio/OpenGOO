#pragma once

#include <QImage>

class QString;
class QFile;

class BinLtlFile
{
    struct Header
    {
        unsigned short width;
        unsigned short height;
        unsigned int compressedSize;
        unsigned int uncompressedSize;
    };

public:
    static QImage Decompress(QFile& aFile);
};
