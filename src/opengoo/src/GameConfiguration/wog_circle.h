#pragma once

#include "wog_pobject.h"

#include <QPointF>

struct WOGCircle : public WOGPObject
{
    QPointF position;
    float radius;
};
