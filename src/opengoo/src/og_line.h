#ifndef OG_LINE_H
#define OG_LINE_H

#include "og_ibody.h"
#include "wog_scene.h"

class OGLine : public OGIBody
{
public:
    OGLine(WOGLine* line, WOGMaterial* material);
    virtual ~OGLine() {}
};

#endif // OG_LINE_H
