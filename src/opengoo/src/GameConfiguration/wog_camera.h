#pragma once

#include <QPointF>

struct WOGPoi
{
    QPointF position;
    float traveltime;
    float pause;
    float zoom;
};

struct WOGCamera
{
    enum Aspect
    {
        Unknown,
        Normal,
        WideScreen
    };

    QPointF endpos;
    float endzoom;
    Aspect aspect;
    QVector<WOGPoi> poi;
};
