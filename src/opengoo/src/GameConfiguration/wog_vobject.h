#pragma once

#include <QColor>
#include <QPoint>

// World of Goo visual object

struct WOGVObject
{
    QString id;
    float depth;
    float x;
    float y;
    float scalex;
    float scaley;
    float rotation;
    float alpha;
    QColor colorize;

    WOGVObject()
        : depth(0.0f)
        , x(0)
        , y(0)
        , scalex(1)
        , scaley(1)
        , rotation(0.0f)
        , alpha(1.0f)
        , colorize(Qt::white)
    {
    }

    virtual ~WOGVObject()
    {
    }
};
