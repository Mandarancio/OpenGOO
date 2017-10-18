#pragma once

#include "og_sprite.h"

class WOGResources;
class OGSprite;
struct WOGPipe;
class QString;
class QPointF;

class SpriteFactory
{
public:    

    static ImageSourcePtr CreateImageSource(const QString& a_id);

    OGSprite* CreateCap(WOGPipe* a_pipe, const QString& a_id, bool a_visible);
    OGSprite* CreateBend(const QString& a_type,
                         const QPointF& a_p1,
                         const QPointF& a_p2,
                         const QPointF& a_p3,
                         float a_depth);
};
