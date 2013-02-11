#ifndef OG_RECTANGLE_H
#define OG_RECTANGLE_H

#include "og_ibody.h"
#include "wog_scene.h"
#include <QPainter>

class OGRectangle : public OGIBody
{
public:
    OGRectangle(WOGRectangle* rect, WOGMaterial* material);
    virtual ~OGRectangle() {}

    void Paint(QPainter* painter, bool debug=false);
};

#endif // OG_RECTANGLE_H
