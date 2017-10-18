#include "og_rectangle.h"
#include "physics.h"
#include "og_world.h"
#include "og_userdata.h"

OGRectangle::OGRectangle(og::physics::PhysicsEngine& a_physicEngine,
                         const WOGRectangle& a_rect,
                         const WOGMaterial& a_material)
    : OGIBody(a_rect, a_material)
{
    if (a_rect.dynamic)
        throw std::logic_error("WOGRectangle is dynamic");

    auto position = a_rect.position;
    auto size = a_rect.size;
    auto angle = a_rect.rotation;

    m_type =  OGIBody::S_RECTANGLE;
    auto data = new OGUserData;
    data->type = OGUserData::GEOM;
    data->data = this;
    auto obj = PhysicsFactory::createRectangle(a_physicEngine, position, size, angle, a_material, false, 0, data);

    body = obj->body;
    fixture = obj->fixture;
    shape = obj->shape;
}

void OGRectangle::_Draw(QPainter* p)
{
    if (debug_)
    {
        const b2Transform& xf = fixture->GetBody()->GetTransform();
        assert(shape->GetType() == og::physics::Shape::e_polygon);
        b2PolygonShape* rect = static_cast<b2PolygonShape*>(shape->GetShape());
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
