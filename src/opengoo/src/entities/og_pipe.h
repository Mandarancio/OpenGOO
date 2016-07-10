#pragma once

#include <memory>

#include <QPointF>
#include <QString>
#include <QImage>

#include "og_ipipe.h"
#include "og_sprite.h"
#include "cap.h"
#include "entityfactory.h"

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

    EntityPtr m_cap;

    virtual void _Close();
    virtual void _Open();
    virtual void _Painter(QPainter*) {}

    OGWorld* GetWorld();

public:
    OGPipe(const WOGPipe& a_pipe, EntityFactory& a_efactory);
};
