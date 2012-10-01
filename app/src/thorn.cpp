#include "thorn.h"
#include "tools.h"

#include <QPainter>
#include <QPoint>
#include <qmath.h>

#include <Box2D/Box2D.h>

static const qreal PI = 3.141593;

Thorn::Thorn(QPoint center, const QList<QPoint>& shape, b2World& world, QObject* parent) :
    Object(parent)
{
    //Body definition
    b2BodyDef def;
    //b2_staticBody ignore all forces and interact only with dynamic body.
    def.type = b2_staticBody;
    //center of the body
    def.position = toVec(center);
    //create the body;
    body = world.CreateBody(&def);
    
    //create the shape
    //make a segment for all the points
    for (QList<QPoint>::const_iterator point1 = shape.begin(), point2 = shape.begin()+1; point1 != shape.end(); ++point1) {
        qreal dx = point1->x() - point2->x();
        qreal dy = point1->y() - point2->y();
        qreal x = (point1->x() + point2->x()) / 20.0;
        qreal y = (point1->y() + point2->y()) / 20.0;

        if (point2 == shape.end())
            point2 = shape.begin();
        else
            ++point2;

        qreal w = qSqrt(dx*dx + dy*dy) / 20.0;
        qreal angle = PI/2.0 - qAtan2(dx,dy);
        
        b2PolygonShape shape;
        shape.SetAsBox(w, 0.1, b2Vec2(x,y), angle);
        body->CreateFixture(&shape, 1.0);
    }
    
    body->SetUserData(this);
    polygon = toPoly(shape, center);
}

void Thorn::paint(QPainter &p)
{
    p.setBrush(Qt::black);
    p.setPen(Qt::transparent);
    p.drawPolygon(polygon);
}
