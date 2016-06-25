#include <memory>

#include <QPointer>

#include "og_pipe.h"
#include "opengoo.h"
#include "og_sprite.h"
#include "wog_pipe.h"
#include "spritefactory.h"


OGPipe::OGPipe(WOGPipe* a_pipe)
{
    SpriteFactory factory;

    QString type = (a_pipe->type.isEmpty() ? "NORMAL" : a_pipe->type);
    auto src = factory.CreateImageSource("IMAGE_GLOBAL_PIPE_" + type);

    auto halfPipeWidth = src->GetHeight() * 0.5f;
    auto verteces = a_pipe->vertex;

    for (int i = 0; i < verteces->size() - 1; i++)
    {
        QPointF p1 = verteces->at(i);
        QPointF p2 = verteces->at(i + 1);
        auto v = p2 - p1;
        float x = p1.x();
        float y = -p1.y();

        auto spr = new OGSprite(x, y, src);
        spr->SetOffsetY(halfPipeWidth);

        if (v.y() == 0)
        {            
            float len = qAbs(v.x());
            float sx = len / src->GetWidth();
            spr->SetScaleX(sx);

            float angle = v.x() > 0 ? 0 : 180;
            spr->SetAngle(angle);

            spr->SetDepth(a_pipe->depth);

            GetWorld()->_InsertSprite(spr);
        }
        else
        {
            float len = qAbs(v.y());
            float sx = len / src->GetWidth();
            spr->SetScaleX(sx);

            float angle = v.y() < 0 ? 90 : 270;
            spr->SetAngle(angle);

            spr->SetDepth(a_pipe->depth);

            GetWorld()->_InsertSprite(spr);
        }
    }

    if (verteces->size() > 2)
    {
        for (int i = 0; i < verteces->size() - 2; i++)
        {
            QPointF p1 = verteces->at(i);
            QPointF p2 = verteces->at(i + 1);
            QPointF p3 = verteces->at(i + 2);

            auto bend = factory.CreateBend(type, p1, p2, p3, a_pipe->depth);

            if (bend)
                GetWorld()->_InsertSprite(bend);
        }
    }

    m_cap[closed] = factory.CreateCap(a_pipe,
                                      "IMAGE_GLOBAL_PIPE_CAP_CLOSED_" + type,
                                      true);

    if (m_cap[closed])
        GetWorld()->_InsertSprite(m_cap[closed]);

    m_cap[open] = factory.CreateCap(a_pipe,
                                    "IMAGE_GLOBAL_PIPE_CAP_OPEN_" + type,
                                    false);

    if (m_cap[open])
        GetWorld()->_InsertSprite(m_cap[open]);
}

void OGPipe::_Close()
{
    m_cap[open]->SetVisible(false);
    m_cap[closed]->SetVisible(true);
}

void OGPipe::_Open()
{
    m_cap[open]->SetVisible(true);
    m_cap[closed]->SetVisible(false);
}

OGWorld* OGPipe::GetWorld()
{
    return OpenGOO::instance()->GetWorld();
}
