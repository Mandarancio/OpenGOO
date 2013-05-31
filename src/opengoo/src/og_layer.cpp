#include "og_layer.h"
#include "og_sprite.h"

#include <QPainter>

OGLayer::OGLayer()
{
}

OGLayer::~OGLayer()
{
//    while(!spriteList_.isEmpty()) delete spriteList_.takeFirst();
}

void OGLayer::Add(OGSprite *sprite)
{
    spriteList_.append(sprite);
}

void OGLayer::Paint(QPainter* painter)
{
    Q_FOREACH (OGSprite* s, spriteList_)
    {
        s->Paint(painter);
    }
}
