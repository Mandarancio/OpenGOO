#include "og_rectangle.h"
#include "physics.h"

OGRectangle::OGRectangle(WOGRectangle* rect, WOGMaterial* material)
    : OGIBody(rect, material)
{
    OGPhysicsBody* obj;

    QPointF position = rect->position;
    QSizeF size = rect->size;
    qreal angle = rect->rotation;

    if (!rect->dynamic)
    {
        m_type =  OGIBody::S_RECTANGLE;
        obj = createRectangle(position, size, angle, material, false, 0
                              , this);
    }

    body = obj->body;
    fixture = obj->fixture;
    shape = obj->shape;
}

void OGRectangle::Paint(QPainter* painter, bool debug)
{
    Q_UNUSED(painter)
    Q_UNUSED(debug)
}
