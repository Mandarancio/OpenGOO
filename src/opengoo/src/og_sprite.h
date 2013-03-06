#ifndef OG_SPRITE_H
#define OG_SPRITE_H

#include "wog_vobject.h"

#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPoint>

struct OGSprite : public WOGVObject
{
    QPixmap image;
    QSize size;
    bool visible;

    void Paint(QPainter* p)
    {
        if (!visible) return;

        p->setOpacity(alpha);

        qreal x = position.x();
        qreal y = position.y();
        qreal w = size.width();
        qreal h = size.height();
        p->drawPixmap(QRectF(x, y, w, h), image, image.rect());
    }
};

#endif // OG_SPRITE_H
