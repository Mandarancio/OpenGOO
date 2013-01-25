#ifndef WOG_VOBJECT_H
#define WOG_VOBJECT_H

#include <QColor>
#include <QPoint>

// World of Goo visual object

struct WOGVObject
{
    qreal depth;
    QPointF position;
    QPointF scale;
    qreal rotation;
    qreal alpha;
    QColor colorize;
};

#endif // WOG_VOBJECT_H
