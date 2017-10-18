#pragma once

#include <QColor>
#include <QPoint>

// World of Goo visual object

struct WOGVObject
{
    float depth;
    QPointF position;
    QPointF scale;
    float rotation;
    float alpha;
    QColor colorize;

    WOGVObject()
        : depth(0.0f)
        , rotation(0.0f)
        , alpha(1.0f)
    {
    }

    virtual ~WOGVObject()
    {
    }
};
