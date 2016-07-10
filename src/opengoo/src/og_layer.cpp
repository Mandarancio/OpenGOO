#include <QPainter>

#include "og_layer.h"
#include "og_sprite.h"

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

void OGLayer::Paint(QPainter& a_painter)
{
    for (int i = 0; i < spriteList_.size(); ++i)
    {
        spriteList_[i]->Paint(&a_painter);
    }
}
