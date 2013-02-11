#include "og_line.h"
#include "physics.h"

OGLine::OGLine(WOGLine *line, WOGMaterial* material)
    : OGIBody(line, material)
{
    OGPhysicsBody* obj;

    QPointF anchor = line->anchor;
    QPointF normal = line->normal;

    if (!line->dynamic)
    {
        obj = createLine(anchor, normal, material, false, this);
    }

    body = obj->body;
    fixture = obj->fixture;
    shape = obj->shape;
}
