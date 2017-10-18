#pragma once

#include <memory>

#include <QList>

class QPainter;
class QVector2D;

namespace og {
class Graphic
{
    Graphic(const Graphic&);
    Graphic& operator=(const Graphic&);

public:
    Graphic()
    {
    }

    virtual ~Graphic()
    {
    }

    virtual void Update() = 0;
    virtual void Render(QPainter& a_painter, const QVector2D& a_pos) = 0;

    virtual float GetAngle() const = 0;
};
}

typedef std::shared_ptr<og::Graphic> GraphicPtr;
typedef QList<GraphicPtr> GraphicList;
