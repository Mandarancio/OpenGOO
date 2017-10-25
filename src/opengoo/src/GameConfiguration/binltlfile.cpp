#include "binltlfile.h"

#include <QFile>

inline static uint powerof2(uint aNumber)
{
    uint n = 1;
    while (aNumber > n)
    {
        n <<= 1;
    }

    return n;
}

QImage BinLtlFile::Decompress(QFile& aFile)
{
    Header header;
    if (aFile.read(reinterpret_cast<char*>(&header), sizeof(Header)) != sizeof(Header))
    {
        return QImage();
    }

    std::vector<char> compressedData(sizeof(int) + header.compressedSize);
    *reinterpret_cast<uint*>(compressedData.data()) = header.uncompressedSize;
    if (aFile.read(compressedData.data() + sizeof(int), header.compressedSize) != header.compressedSize)
    {
        return QImage();
    }

    auto uncompressedData = qUncompress(reinterpret_cast<uchar*>(compressedData.data()), compressedData.size());
    auto w = powerof2(header.width);
    auto h = powerof2(header.height);
    auto sz = qMax(w, h);
    QImage img(header.width, header.height, QImage::Format_RGBA8888);
    for (uint y = 0; y < header.height; ++y)
    {
        for (uint x = 0; x < header.width; ++x)
        {
            auto col = reinterpret_cast<uint*>(uncompressedData.data())[x + y * sz];
            auto r = qBlue(col);
            auto g = qGreen(col);
            auto b = qRed(col);
            auto a = qAlpha(col);
            auto rgba = qRgba(r, g, b, a);
            img.setPixel(x, y, rgba);
        }
    }

    return img;
}
