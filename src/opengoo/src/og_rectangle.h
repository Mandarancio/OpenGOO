#ifndef OG_RECTANGLE_H
#define OG_RECTANGLE_H

#include "og_ibody.h"
#include "wog_scene.h"
#include <QPainter>

class OGRectangle : public OGIBody
{
    void _Draw(QPainter* p);

public:
    OGRectangle(WOGRectangle* rect, WOGMaterial* material);
    virtual ~OGRectangle() {}    
};

#endif // OG_RECTANGLE_H
