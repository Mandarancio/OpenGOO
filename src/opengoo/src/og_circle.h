#ifndef OG_CIRCLE_H
#define OG_CIRCLE_H

#include "og_ibody.h"
#include "wog_scene.h"


class OGCircle : public OGIBody
{
        void _Draw(QPainter* p);

    public:
        OGCircle(WOGCircle* circle, WOGMaterial* material);
        virtual ~OGCircle() {}
};

#endif // OG_CIRCLE_H
