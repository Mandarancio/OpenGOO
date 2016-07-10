#pragma once
#include <QList>

class OGSprite;
class QPainter;

typedef QList<OGSprite*> OGSpriteList;

class OGLayer
{
public:
    OGLayer();
    ~OGLayer();

    void Add(OGSprite* sprite);
    void Paint(QPainter& a_painter);

private:
    OGSpriteList spriteList_;
};
