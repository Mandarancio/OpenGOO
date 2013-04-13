#ifndef WOG_VOBJECT_H
#define WOG_VOBJECT_H

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

    WOGVObject() : alpha(1) {}
};

#endif // WOG_VOBJECT_H
