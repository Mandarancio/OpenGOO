#ifndef OG_LAYER_H
#define OG_LAYER_H

#include <QList>

struct OGSprite;

typedef QList<OGSprite*> OGSpriteList;


class QPainter;

class OGLayer
{
public:
    OGLayer();
    ~OGLayer();

    void Add(OGSprite* sprite);
    void Paint(QPainter* painter);

private:
    OGSpriteList spriteList_;
};

#endif // OG_LAYER_H
