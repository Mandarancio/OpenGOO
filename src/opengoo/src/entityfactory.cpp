#include <memory>

#include "opengoo.h"
#include "entityfactory.h"
#include "spritefactory.h"

#include "entities/cap.h"
#include "wog_pipe.h"
#include "entities/og_pipe.h"
#include "entities/gamecontroller.h"

#include "entities/exit.h"

#define CAP_OPEN "IMAGE_GLOBAL_PIPE_CAP_OPEN_"
#define CAP_CLOSED "IMAGE_GLOBAL_PIPE_CAP_CLOSED_"

#define PIPE_BEND "IMAGE_GLOBAL_PIPE_BEND_"


static inline QString BuildPrefix(const QPointF& a_p1,
                                  const QPointF& a_p2,
                                  const QPointF& a_p3)
{
    auto v1 = a_p2 - a_p1;
    auto v2 = a_p3 - a_p2;

    if (v1.y() > 0 || v2.y() < 0)
        return (v2.x() < 0 || v1.x() > 0 ? "TR_" : "TL_");
    else
        return (v2.x() < 0 || v1.x() > 0 ? "BR_" : "BL_");
}

EntityPtr EntityFactory::CreateBend(const QString& a_type,
                     const QPointF& a_p1,
                     const QPointF& a_p2,
                     const QPointF& a_p3,
                     float a_depth)
{
    auto prefix = BuildPrefix(a_p1, a_p2, a_p3);
    auto id = PIPE_BEND + prefix + a_type;

    auto src = SpriteFactory::CreateImageSource(id);
    auto spr = std::make_shared<OGSprite>(src);
    spr->CenterOrigin();
    spr->SetDepth(a_depth);

    auto e = std::make_shared<og::Entity>((float)a_p2.x(), (float)-a_p2.y(), spr);

    return e;
}

EntityPtr EntityFactory::CreateCap(const WOGPipe& a_pipe,
                                   const QString& a_type)
{
    if (a_pipe.vertex->size() < 2)
        throw std::invalid_argument("vertices less than 2");

    QPointF p1 = a_pipe.vertex->at(0);
    QPointF p2 = a_pipe.vertex->at(1);
    auto v = p2 - p1;
    float angle = (v.x() == 0 ? (v.y() > 0 ? 0 : 180) : (v.x() > 0 ? 90 : -90));

    float x = p1.x();
    float y = -p1.y();

    GraphicList l;

    auto src = SpriteFactory::CreateImageSource(CAP_OPEN + a_type);
    auto spr = OGSprite::Create(src);
    spr->CenterOrigin();
    spr->SetAngle(angle);
    spr->SetDepth(a_pipe.depth);
    l.append(spr);

    src = SpriteFactory::CreateImageSource(CAP_CLOSED + a_type);
    spr = OGSprite::Create(src);
    spr->CenterOrigin();
    spr->SetAngle(angle);
    spr->SetDepth(a_pipe.depth);
    l.append(spr);

    return std::make_shared<Cap>(x, y, l);
}

OGIPipe* EntityFactory::CreatePipe(const WOGPipe& a_pipe)
{
    return new OGPipe(a_pipe, *this);
}

EntityPtr EntityFactory::CreateGameController(GameData& a_gdata)
{
    return std::make_shared<GameController>(a_gdata);
}

EntityPtr EntityFactory::CreateExit(const WOGLevelExit& a_exit,
                                ExitEventListener* a_listener)
{
    return std::make_shared<Exit>(a_exit, a_listener);
}
