#include <memory>

#include <QPointer>

#include "og_pipe.h"
#include "opengoo.h"
#include "og_sprite.h"
#include "wog_pipe.h"
#include "spritefactory.h"

OGPipe::OGPipe(const WOGPipe& a_pipe, EntityFactory& a_efactory)
{
    QString type = (a_pipe.type.isEmpty() ? "NORMAL" : a_pipe.type);
    auto src = SpriteFactory::CreateImageSource("IMAGE_GLOBAL_PIPE_" + type);

    auto halfPipeWidth = src->GetHeight() * 0.5f;
    auto verteces = a_pipe.vertex;

    for (int i = 0; i < verteces.size() - 1; i++)
    {
        QPointF p1 = verteces[i];
        QPointF p2 = verteces[i + 1];
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

            spr->SetDepth(a_pipe.depth);

//            FIXME
//            GetWorld()->_InsertSprite(spr);
        }
        else
        {
            float len = qAbs(v.y());
            float sx = len / src->GetWidth();
            spr->SetScaleX(sx);

            float angle = v.y() < 0 ? 90 : 270;
            spr->SetAngle(angle);

            spr->SetDepth(a_pipe.depth);

//            FIXME
//            GetWorld()->_InsertSprite(spr);
        }
    }

    assert(verteces.size() > 2);
    for (int i = 0; i < verteces.size() - 2; i++)
    {
        QPointF p1 = verteces[i];
        QPointF p2 = verteces[i + 1];
        QPointF p3 = verteces[i + 2];

        auto bend = a_efactory.CreateBend(type, p1, p2, p3, a_pipe.depth);
        GetWorld()->AddEntity(bend);
    }

    m_cap = a_efactory.CreateCap(a_pipe, type);
    GetWorld()->AddEntity(m_cap);
}

void OGPipe::_Close()
{
    static_cast<Cap&>(*m_cap).SetOpen(false);
}

void OGPipe::_Open()
{
    static_cast<Cap&>(*m_cap).SetOpen(true);
}

OGWorld* OGPipe::GetWorld()
{
    return OpenGOO::GetInstance()->GetWorld();
}
