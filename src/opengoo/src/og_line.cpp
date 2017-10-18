#include "og_line.h"
#include "physics.h"
#include "og_userdata.h"

OGLine::OGLine(og::physics::PhysicsEngine& a_physicEngine, const WOGLine& line, const WOGMaterial& material)
    : OGIBody(line, material)
{
    if (line.dynamic)
        throw std::logic_error("WOGLine is not static");

    auto& anchor = line.anchor;
    auto& normal = line.normal;

    auto data = new OGUserData;
    data->type = OGUserData::GEOM;
    data->data = this;

    auto obj = PhysicsFactory::createLine(a_physicEngine, anchor, normal, material, false, data);

    body = obj->body;
    fixture = obj->fixture;
    shape = obj->shape;

    delete obj;
}
