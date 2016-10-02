#include <memory>

#include <Box2D/Box2D.h>

#include "PhysicsEngine/distancejoint.h"

#include "GameEngine/og_gameengine.h"
#include "GameEngine/Colliders/circlecollider.h"

#include "opengoo.h"
#include "entityfactory.h"
#include "spritefactory.h"

#include "wog_pipe.h"
#include "wog_scene.h"
#include "wog_level.h"

#include "physics.h"

#include "entities/cap.h"
#include "entities/gamecontroller.h"
#include "entities/exit.h"
#include "entities/radialforcefield.h"
#include "entities/og_pipe.h"
#include "entities/og_ball.h"
#include "entities/strand.h"

#include "ijointbuilder.h"

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

EntityPtr EntityFactory::CreateRadialForceField(const WOGRadialForceField& a_field)
{
    return RadialForceField::Builder()
            .SetCenter(a_field.center)
            .SetRadius(a_field.radius)
            .SetForceAtCenter(a_field.forceatcenter)
            .SetForceAtEdge(a_field.forceatedge)
            .SetCategory(PhysicsFactory::SENSOR)
            .SetMask(PhysicsFactory::BALL)
            .Build();
}

EntityPtr EntityFactory::CreateBall(const WOGBallInstance& a_ball)
{
    auto ballDef = GE->getResourceManager()->GetBallByType(a_ball.type);
    if (!ballDef)
        return nullptr;

    auto ball = OGBall::Builder().SetInstDef(a_ball).SetBallDef(ballDef).Build();
    auto shape = ballDef->attribute.core.shape;
    switch (shape->type)
    {
    case WOGBallShape::e_circle:
        ball->SetCollider(std::make_shared<og::CircleCollider>(static_cast<WOGCircleBall&>(*shape).radius, ball.get()));
        break;
    default:
        assert(false);
    }

    ball->GetBody()->SetActive(false);

    return ball;
}

struct JointBuilder : IJointBuilder
{
    JointBuilder()
    {
        m_ball1 = nullptr;
        m_ball2 = nullptr;
    }

    JointBuilder& SetType(const QString& a_type)
    {
        m_type = a_type;
        return *this;
    }

    JointBuilder& SetBall1(OGBall& a_ball)
    {
        m_ball1 = a_ball.GetBody();
        return *this;
    }

    JointBuilder& SetBall2(OGBall& a_ball)
    {
        m_ball2 = a_ball.GetBody();
        return *this;
    }

    JointUPtr Build()
    {
        if (m_type == "spring")
            return JointUPtr(new og::physics::DistanceJoint(m_ball1, m_ball2, (OGUserData*)nullptr));

        assert(false);
        return nullptr;
    }

    og::PhysicsBody* m_ball1;
    og::PhysicsBody* m_ball2;
    QString m_type;
};

EntityPtr EntityFactory::CreateStrand(EntityPtr a_ball1, EntityPtr a_ball2)
{
    auto& ball1 = static_cast<OGBall&>(*a_ball1);
    auto& ball2 = static_cast<OGBall&>(*a_ball2);

    auto rm = GE->getResourceManager();

    auto ballDef1 = rm->GetBallByType(ball1.GetName());
    auto ballDef2 = rm->GetBallByType(ball2.GetName());

    if(!ballDef1->strand || !ballDef2->strand)
        return nullptr;

    if(ballDef1->strand->type != ballDef2->strand->type)
        return nullptr;

    auto src = rm->GetImageSourceById(ballDef1->strand->image);
    auto spr = OGSprite::Create(src);
    spr->SetOffsetX(src->GetWidth() * 0.5f);

    JointBuilder builder;
    builder.SetBall1(ball1).SetBall2(ball2).SetType(ballDef1->strand->type);

    return std::make_shared<Strand>(spr, builder);
}
