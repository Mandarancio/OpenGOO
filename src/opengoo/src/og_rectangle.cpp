#include "og_rectangle.h"
#include "physics.h"
#include "og_world.h"

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
        OGUserData* data = new OGUserData;
        data->type = OGUserData::GEOM;
        data->data = this;
        obj = createRectangle(position, size, angle, material, false, 0
                              , data);
    }

    body = obj->body;
    fixture = obj->fixture;
    shape = obj->shape;
}

void OGRectangle::_Draw(QPainter* p)
{
    if (debug_)
    {
        const b2Transform& xf = fixture->GetBody()->GetTransform();
        b2PolygonShape* rect = static_cast<b2PolygonShape*>(shape->shape);
        int vertexCount = rect->m_vertexCount;

        b2Vec2 v;
        QPointF points[4];

        for (int i = 0; i < vertexCount; ++i)
        {
            v = b2Mul(xf, rect->m_vertices[i]);
            points[i] = QPointF(v.x * 10, v.y * 10 * -1);
        }

        QColor greenColor(0, 255, 0, 100);

        p->save();

        p->setPen(Qt::transparent);
        p->setBrush(greenColor);
        p->drawPolygon(points, 4);

        p->restore();
    }
}
