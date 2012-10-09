#ifndef THORN_H
#define THORN_H

#include "object.h"

#include <QList>
#include <QPolygon>

class QPoint;
class QPainter;

class b2World;


class Thorn : public Object
{
    Q_OBJECT

public:
    Thorn(QPoint center, const QList<QPoint>& shape, b2World& world, QObject* parent = 0);
    
    QRect boundingRect()
    {
        return polygon.boundingRect();
    }

private:
    QPolygon polygon;

signals:

public slots:
    void paint(QPainter &p);

};

#endif // THORN_H
