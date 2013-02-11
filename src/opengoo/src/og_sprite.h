#ifndef OG_SPRITE_H
#define OG_SPRITE_H

#include "wog_vobject.h"

#include <QPixmap>
#include <QImage>

struct OGSprite : public WOGVObject
{
    QPixmap image;
    QSize size;
    bool visible;
};

#endif // OG_SPRITE_H
