#pragma once

#include <memory>

#include <QPointF>
#include <QString>
#include <QImage>

#include "og_ipipe.h"
#include "og_sprite.h"


struct WOGPipe;
class OGWorld;
class QPainter;

class OGPipe : public OGIPipe
{
    enum
    {
        closed,
        open
    };

    OGSprite* m_cap[2];

    virtual void _Close();
    virtual void _Open();
    virtual void _Painter(QPainter*) {}

    OGWorld* GetWorld();

public:
    OGPipe(WOGPipe* a_pipe);
};
