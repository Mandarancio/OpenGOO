#include "og_line.h"
#include "physics.h"
#include "og_userdata.h"

OGLine::OGLine(WOGLine *line, WOGMaterial* material)
    : OGIBody(line, material)
{
    OGPhysicsBody* obj;

    QPointF anchor = line->anchor;
    QPointF normal = line->normal;

    if (!line->dynamic)
    {
        OGUserData* data = new OGUserData;
        data->type = OGUserData::GEOM;
        data->data = this;
        obj = createLine(anchor, normal, material, false, data);
    }

    body = obj->body;
    fixture = obj->fixture;
    shape = obj->shape;
}
