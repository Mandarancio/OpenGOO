#include <QString>

#include "opengoo.h"
#include "wog_pipe.h"
#include "wog_resources.h"

#include "spritefactory.h"


WOGResources* SpriteFactory::GetResourceManager()
{
    return OpenGOO::GetInstance()->GetWorld()->resrcdata();
}

ImageSourcePtr SpriteFactory::CreateImageSource(const QString& a_id)
{
    WOGResources* rm = GetResourceManager();
    QString filename = rm->GetImage(a_id) + ".png";

    return std::make_shared<og::ImageSource>(filename);
}

OGSprite* SpriteFactory::CreateCap(WOGPipe* a_pipe,
                                   const QString& a_id,
                                   bool a_visible)
{
    auto src = CreateImageSource(a_id);

    if (!src || a_pipe->vertex->size() < 2)
        return nullptr;

    QPointF p1 = a_pipe->vertex->at(0);
    QPointF p2 = a_pipe->vertex->at(1);
    auto v = p2 - p1;
    float angle = (v.x() == 0 ? (v.y() > 0 ? 0 : 180) : (v.x() > 0 ? 90 : -90));

    float x = p1.x();
    float y = -p1.y();

    auto spr = std::unique_ptr<OGSprite>(new OGSprite(x, y, src));
    spr->CenterOrigin();
    spr->SetAngle(angle);
    spr->SetVisible(a_visible);
    spr->SetDepth(a_pipe->depth);

    return spr.release();
}

OGSprite* SpriteFactory::CreateBend(const QString& a_type,
                     const QPointF& a_p1,
                     const QPointF& a_p2,
                     const QPointF& a_p3,
                     float a_depth)
{
    auto v1 = a_p2 - a_p1;
    auto v2 = a_p3 - a_p2;

    QString id = "IMAGE_GLOBAL_PIPE_BEND_";

    if (v1.y() > 0 || v2.y() < 0)
        id += (v2.x() < 0 || v1.x() > 0 ? "TR_" + a_type : "TL_" + a_type);
    else
        id += (v2.x() < 0 || v1.x() > 0 ? "BR_" + a_type : "BL_" + a_type);

    auto src = CreateImageSource(id);

    if (!src)
        return nullptr;

    auto spr = std::unique_ptr<OGSprite>(new OGSprite(src));
    spr->SetPosition(a_p2.x(), -a_p2.y());
    spr->SetDepth(a_depth);
    spr->CenterOrigin();

    return spr.release();
}
