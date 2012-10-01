#ifndef TOOLS_H
#define TOOLS_H

#include <Box2D/Box2D.h>

#include <QList>
#include <QPoint>

class QPolygon;
class QString;

// Conversion for qt/box2d

//! Convert a QPoint in a b2Vec2
inline b2Vec2 toVec(QPoint p)
{
    return b2Vec2(p.x()/10.0, p.y()/10.0);
}

//! Convert a b2Vec2 in a QPoint
inline QPoint toPoint(b2Vec2 v)
{
    return QPoint(qRound(v.x*10.0), qRound(v.y*10.0));
}

//! Convert a QList of QPoint in a QPolygon transated of *center*
QPolygon toPoly(const QList<QPoint>& ps, QPoint center);

QString time2string(int time);

#endif // TOOLS_H
