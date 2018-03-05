#include "og_circle.h"
#include "physics.h"
#include "wog_scene.h"
#include "wog_material.h"
#include "og_userdata.h"

#include <QPainter>

OGCircle::OGCircle(og::physics::PhysicsEngine& a_physicEngine, const WOGCircle& a_circle, const WOGMaterial& a_material)
    : OGIBody(a_circle, a_material)
{
    if (a_circle.dynamic)
        throw std::logic_error("WOGCircle is dynamic");

    auto& position = a_circle.position;
    auto radius = a_circle.radius;

    auto obj = PhysicsFactory::createCircle(a_physicEngine,
                                            position,
                                            radius,
                                            0,
                                            a_material,
                                            false,
                                            0,
                                            new OGUserData(OGUserData::GEOM, this));

//    body = obj->body;
//    fixture = obj->fixture;
//    shape = obj->shape;

    delete obj;
}

void OGCircle::_Draw(QPainter* p)
{
//    if (debug_)
//    {
//        float r = shape->GetRadius() * 10;
//        b2Vec2 pos = body->GetPosition();
//        float posX = pos.x * 10;
//        float posY = pos.y * 10* -1;

//        p->save();

//        p->setPen(Qt::yellow);
//        p->drawEllipse(QPointF(posX, posY), r, r);

//        p->restore();
//    }
}
