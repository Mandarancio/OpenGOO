#ifndef WOG_CIRCLE_H
#define WOG_CIRCLE_H

#include "wog_pobject.h"

#include <QPointF>

struct WOGCircle : public WOGPObject
{
    QPointF position;
    float radius;
};

#endif // WOG_CIRCLE_H
