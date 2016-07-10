#include "og_circle.h"
#include "physics.h"
#include "wog_scene.h"
#include "wog_material.h"
#include "og_userdata.h"

#include <QPainter>

OGCircle::OGCircle(WOGCircle* circle, WOGMaterial* material)
    : OGIBody(circle, material)
{
    PhysicsBody* obj;

    QPointF position = circle->position;
    float32 radius = circle->radius;

    if (!circle->dynamic)
    {
        m_type =  OGIBody::S_CIRCLE;
        OGUserData* data = new OGUserData;
        data->type = OGUserData::GEOM;
        data->data = this;
        obj = PhysicsFactory::createCircle(position, radius, 0, material, false, 0, data);
    }

    body = obj->body;
    fixture = obj->fixture;
    shape = obj->shape;
}

void OGCircle::_Draw(QPainter* p)
{
    if (debug_)
    {
        float r = shape->GetRadius() * 10;
        b2Vec2 pos = body->GetPosition();
        float posX = pos.x * 10;
        float posY = pos.y * 10* -1;

        p->save();

        p->setPen(Qt::yellow);
        p->drawEllipse(QPointF(posX, posY), r, r);

        p->restore();
    }
}
