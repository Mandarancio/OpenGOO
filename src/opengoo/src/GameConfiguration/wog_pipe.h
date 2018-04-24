#pragma once

#include <QVector>
#include <QPoint>
#include <QString>

struct WOGPipe
{
    struct WOGVertex
    {
        int x;
        int y;
    };

    QString id;
    QString type;
    float depth;
    QVector<WOGVertex> vertex;

    WOGPipe()
        : id(QLatin1String("0"))
        , depth(0)
    {
    }
};
