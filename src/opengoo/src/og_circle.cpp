#include "og_circle.h"
#include "physics.h"

OGCircle::OGCircle(WOGCircle *circle, WOGMaterial* material)
    : OGIBody(circle, material)
{
    OGPhysicsBody* obj;

    QPointF position = circle->position;
    float32 radius = circle->radius;

    if (!circle->dynamic)
    {
        m_type =  OGIBody::S_CIRCLE;
        OGUserData* data = new OGUserData;
        data->type = OGUserData::GEOM;
        data->data = this;
        obj = createCircle(position, radius, 0, material, false, 0, data);
    }

    body = obj->body;
    fixture = obj->fixture;
    shape = obj->shape;
}

