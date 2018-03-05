#include <memory>

#include <QDebug>
#include <QtMath>

#include "OGLib/util.h"

//#include "PhysicsEngine/distancejoint.h"
#include "PhysicsEngine/physicsbodybuilder.h"

#include "GameEngine/og_gameengine.h"
#include "GameEngine/Colliders/circlecollider.h"
#include "GameEngine/Colliders/rectcollider.h"

#include "opengoo.h"
#include "entityfactory.h"
#include "multisprite.h"
#include "circlesprite.h"
#include "rectsprite.h"
#include "linesprite.h"

#include "wog_pipe.h"
#include "wog_scene.h"
#include "wog_level.h"
#include "wog_ball.h"

#include "physics.h"

#include "entities/cap.h"
#include "entities/gamecontroller.h"
#include "entities/exit.h"
#include "entities/radialforcefield.h"
#include "entities/ball.h"
#include "entities/strand.h"
#include "entities/button.h"
#include "entities/pipe.h"

#include "ijointbuilder.h"

#include "flags.h"

inline static og::ImageSourceSPtr GetImageSourceById(const QString& aId)
{
    return GE->GetResourceManager()->GetImageSourceById(aId);
}

EntityPtr EntityFactory::CreatePipe(const WOGPipe& a_pipe)
{
    return std::make_shared<Pipe>(a_pipe);
}

EntityPtr EntityFactory::CreateGameController()
{
    return std::make_shared<GameController>();
}

EntityPtr EntityFactory::CreateExit(const WOGLevelExit& a_exit, ExitEventListener* a_listener)
{
    return std::make_shared<Exit>(*GetPhysicsEngine(), a_exit, a_listener);
}

EntityPtr EntityFactory::CreateRadialForceField(const WOGRadialForceField& a_field)
{
    return RadialForceField::Builder(GetPhysicsEngine())
            .SetCenter(a_field.center)
            .SetRadius(a_field.radius)
            .SetForceAtCenter(a_field.forceatcenter)
            .SetForceAtEdge(a_field.forceatedge)
            .SetCategory(PhysicsFactory::SENSOR)
            .SetMask(PhysicsFactory::BALL)
            .Build();
}

std::shared_ptr<Ball> EntityFactory::CreateBall(const WOGBallInstance& a_ball)
{
    auto ballDef = GE->GetResourceManager()->GetBallByType(a_ball.type);
    if (!ballDef)
        return nullptr;

#if 0
    BodyBuilder builder(GetPhysicsEngine());
    builder.SetPosition(QVector2D(a_ball.x,a_ball.y));
    builder.SetAngle(a_ball.angle);

    const auto& shape = ballDef->attribute.core.shape;
    auto type = GetBodyType(shape->type);
    builder.SetType(type);
    builder.SetVariation(shape->variation);
    builder.SetMass(ballDef->attribute.core.mass);

    WOGMaterial m;
    m.bounce = 0.1f;
    m.friction = 15.0f;
    builder.SetMaterial(m);

    switch (type)
    {
    case BodyBuilder::e_circle:
        builder.SetRadius(static_cast<WOGCircleBall&>(*shape).radius);
        break;
    case BodyBuilder::e_rect:
        break;
    case BodyBuilder::e_unknown:
        break;
    }

    auto& sounds = ballDef->sounds;
    QMap<QString, std::shared_ptr<og::audio::SoundSourceList>> soundMap;
    for (auto it = sounds.cbegin(); it != sounds.cend(); ++it)
    {
        auto sl = std::make_shared<og::audio::SoundSourceList>();
        foreach (const auto& id, it.value())
        {
            auto src= GE->GetResourceManager()->GetSoundSource(id);
            sl->Add(src);
        }

        soundMap[it.key()] = sl;
    }

    auto ball = std::make_shared<OGBall>(GetPhysicsEngine(), a_ball, ballDef, builder, soundMap);

    foreach(const auto& part, ballDef->parts)
    {
        const auto& name = part.name;
        if (name == "body")
        {
            auto src = rm->GetImageSourceById(part.image);
            auto spr = OGSprite::Create(src);
            spr->CenterOrigin();
            ball->SetGraphic(spr);
            break;
        }
    }

    std::shared_ptr<og::Collider> col;
    switch (type)
    {
    case BodyBuilder::e_circle:
        col = std::make_shared<og::CircleCollider>(static_cast<WOGCircleBall&>(*shape).radius);
        break;
    case BodyBuilder::e_rect:
        break;
    case BodyBuilder::e_unknown:
        break;
    }

    ball->SetCollider(col);
    ball->GetBody()->SetActive(false);

    return ball;
#else
    auto multiSpr = std::make_shared<MultiSprite>(false);
    foreach(const auto& part, ballDef->parts)
    {
        const auto& image = part.image[GE->GetRandomGenerator()->Range(0, part.image.size() - 1)];
        auto src = GetImageSourceById(image);
        if (!src)
        {
            continue;
        }

        auto x = part.x.size() == 1 ? part.x[0] : GE->GetRandomGenerator()->Range(part.x[0], part.x[1]);
        auto y = part.y.size() == 1 ? part.y[0] : GE->GetRandomGenerator()->Range(part.y[0], part.y[1]);
        x += src->GetWidth() * 0.5f;
        y += src->GetHeight() * 0.5f;

        auto spr = OGSprite::Create(src);
        spr->SetOffsetX(x);
        spr->SetOffsetY(y);
        spr->SetScaleX(part.scale);
        spr->SetScaleY(part.scale);
        multiSpr->AddSprite(std::move(spr), part.layer);
    }

    auto entity = std::make_shared<Ball>(multiSpr);
    entity->SetName(a_ball.type);

    {
        using namespace og::physics;
        PhysicsBodyBuilder builder(GetPhysicsEngine());
        builder.SetBodyType(BodyDef::e_dynamic).SetAngle(a_ball.angle).SetPosition(a_ball.x, a_ball.y);

        if (ballDef->attribute.core.shape.type == WOGBallShape::e_circle)
        {
            builder.SetShapeType(Shape::e_circle).SetRadius(ballDef->attribute.core.shape.radius);
        }
        else if (ballDef->attribute.core.shape.type == WOGBallShape::e_rectangle)
        {
            builder.SetShapeType(Shape::e_polygon)
                    .SetSize(ballDef->attribute.core.shape.width, ballDef->attribute.core.shape.height);
        }
        else
        {
            return entity;
        }

        entity->SetPhysicsBody(builder.Build());
    }

    return entity;
#endif
}

//struct JointBuilder : IJointBuilder
//{
//    JointBuilder(og::physics::PhysicsEngine* a_physicEngine)
//        : m_ball1(nullptr)
//        , m_ball2(nullptr)
//        , m_physicEngine(a_physicEngine)
//    {
//    }

//    JointBuilder& SetType(const QString& a_type)
//    {
//        m_type = a_type;
//        return *this;
//    }

//    JointBuilder& SetBall1(OGBall& a_ball)
//    {
//        m_ball1 = a_ball.GetBody();
//        return *this;
//    }

//    JointBuilder& SetBall2(OGBall& a_ball)
//    {
//        m_ball2 = a_ball.GetBody();
//        return *this;
//    }

//    JointUPtr Build()
//    {
//        if (m_type == QLatin1String("spring"))
//        {
//            return JointUPtr(new og::physics::DistanceJoint(*m_physicEngine, m_ball1, m_ball2, (OGUserData*)nullptr));
//        }

//        return nullptr;
//    }

//    og::PhysicsBody* m_ball1;
//    og::PhysicsBody* m_ball2;
//    QString m_type;
//    og::physics::PhysicsEngine* m_physicEngine;
//};

EntityPtr EntityFactory::CreateStrand(EntityPtr a_ball1, EntityPtr a_ball2)
{
//    auto& ball1 = static_cast<OGBall&>(*a_ball1);
//    auto& ball2 = static_cast<OGBall&>(*a_ball2);

//    auto ballDef1 = GE->GetResourceManager()->GetBallByType(ball1.GetName());
//    auto ballDef2 = GE->GetResourceManager()->GetBallByType(ball2.GetName());

//    if(!ballDef1->strand || !ballDef2->strand)
//        return nullptr;

//    if(ballDef1->strand->type != ballDef2->strand->type)
//        return nullptr;

//    auto src = GetImageSourceById(ballDef1->strand->image);
//    auto spr = OGSprite::Create(src);
//    spr->SetOffsetX(src->GetWidth() * 0.5f);

//    JointBuilder builder(GetPhysicsEngine());
//    builder.SetBall1(ball1).SetBall2(ball2).SetType(ballDef1->strand->type);

//    return std::make_shared<Strand>(spr, builder);
    return nullptr;
}

EntityPtr EntityFactory::CreateStrand(EntityPtr aEntity1, EntityPtr aEntity2, const WOGBallStrand& aDef)
{
    using namespace og::physics;
    JointDef jointDef;
    if (aDef.type == QLatin1String("spring"))
    {
        jointDef.type = Joint::e_distance;
    }
    else
    {
        qDebug() << aDef.type;
        return nullptr;
    }

    auto joint = GetPhysicsEngine()->CreateJoint(aEntity1->GetPhysicsBody(), aEntity2->GetPhysicsBody(), jointDef);
    if (!joint)
    {
        return nullptr;
    }

    auto src = GE->GetResourceManager()->GetImageSourceById(aDef.image);
    if (!src)
    {
        return nullptr;
    }

    const float STRAND_WIDTH = 32.0f;

    auto spr = OGSprite::Create(src);
    spr->SetOffsetX(src->GetWidth() * 0.5f);
    spr->SetScaleX(STRAND_WIDTH / src->GetWidth());
    auto entity = std::make_shared<Strand>(spr, std::move(joint));
    entity->SetDepth(-1);

    return entity;
}

class SpriteBuilder
{
public:
    SpriteBuilder()
    {
        m_scaleX = m_scaleY = 1.0f;
        m_angle = 0.0f;
        m_isCenterOrigin = false;
    }

    SpriteBuilder& SetSource(og::ImageSourceSPtr& a_src)
    {
        m_imageSource = a_src;

        return *this;
    }

    SpriteBuilder& SetCenterOrigin(bool a_flag)
    {
        m_isCenterOrigin = a_flag;

        return *this;
    }

    SpriteBuilder& SetAngle(float a_angle)
    {
        m_angle = a_angle;

        return *this;
    }

    SpriteBuilder& SetScale(const QPointF& a_scale)
    {
        m_scaleX = a_scale.x();
        m_scaleY = a_scale.y();

        return *this;
    }

    std::shared_ptr<OGSprite> Build()
    {
        auto spr = OGSprite::Create(m_imageSource);

        if (m_isCenterOrigin)
            spr->CenterOrigin();

        spr->SetAngle(-m_angle);
        spr->SetScaleX(m_scaleX);
        spr->SetScaleY(m_scaleY);

        return spr;
    }

private:
    og::ImageSourceSPtr m_imageSource;
    float m_scaleX;
    float m_scaleY;
    float m_angle;
    bool m_isCenterOrigin;
};

class ButtonBuilder
{
public:
    ButtonBuilder()
        : m_position()
        , m_id()
        , m_scale()
        , m_up()
        , m_over()
        , m_rotation(0.0f)
        , m_depth(0)
    {
    }

    ButtonBuilder& SetPosition(const QPointF& a_position)
    {
        m_position = a_position;
        return *this;
    }

    ButtonBuilder& SetId(const QString& a_id)
    {
        m_id = a_id;
        return *this;
    }

    ButtonBuilder& SetRotation(float a_rotation)
    {
        m_rotation = a_rotation;
        return *this;
    }

    ButtonBuilder& SetScale(const QPointF& a_scale)
    {
        m_scale = a_scale;
        return *this;
    }

    ButtonBuilder& SetUp(const QString& a_up)
    {
        m_up = a_up;
        return *this;
    }

    ButtonBuilder& SetOver(const QString& a_over)
    {
        m_over = a_over;
        return *this;
    }

    ButtonBuilder& SetCallback(const Button::Callback& a_callback)
    {
        m_callback = a_callback;
        return *this;
    }

    ButtonBuilder& SetDepth(float a_depth)
    {
        m_depth = a_depth;
        return *this;
    }

    std::shared_ptr<Button> Build()
    {
        auto up = CreateSprite(m_up);
        auto over = CreateSprite(m_over);

        auto col = std::make_shared<og::RectCollider>(up->GetScaledWidth(), up->GetScaledHeight());
        auto b = std::make_shared<Button>(m_position.x(), -m_position.y(), up, over, m_callback);
        b->SetName(m_id);
        b->SetCollider(col);
        b->SetDepth(m_depth);
        return b;
    }

private:
    std::shared_ptr<OGSprite> CreateSprite(const QString& a_id)
    {
        auto src = GetImageSourceById(a_id);
        if (!src)
        {
            src = std::make_shared<og::ImageSource>();
        }

        return SpriteBuilder()
            .SetSource(src)
            .SetCenterOrigin(true)
            .SetAngle(m_rotation)
            .SetScale(m_scale)
            .Build();
    }

private:
    QPointF m_position;
    QString m_id;
    QPointF m_scale;
    QString m_up;
    QString m_over;
    float m_rotation;
    float m_depth;
    Button::Callback m_callback;
};

EntityPtr EntityFactory::CreateButton(const WOGButton& a_btnDef)
{
    ButtonBuilder builder;
    builder.SetId(a_btnDef.id)
            .SetPosition(a_btnDef.position)
            .SetUp(a_btnDef.up)
            .SetOver(a_btnDef.over)
            .SetScale(a_btnDef.scale)
            .SetRotation(a_btnDef.rotation);

    if (a_btnDef.id == QLatin1String("quit"))
    {        
        builder.SetCallback([](og::Entity&){ GAME->Quit(); });
    }
    else if (a_btnDef.id.startsWith(QLatin1String("island")))
    {
        builder.SetCallback([](og::Entity& a_e){ GAME->GotoScene(a_e.GetName()); });
    }
    else if (a_btnDef.id.startsWith(QLatin1String("lb_")))
    {
        builder.SetCallback([](og::Entity& a_e){ GAME->GotoScene(a_e.GetName().mid(3)); });
    }
    else if (a_btnDef.id == QLatin1String("changename"))
    {
        // TOOLTIP_PROFILES
    }
    else if (a_btnDef.id == QLatin1String("credits"))
    {
        // TOOLTIP_CREDITS
    }

    return builder.Build();
}

EntityPtr EntityFactory::CreateLabel(const WOGLabel& a_label)
{
    if (auto fnt = GE->GetResourceManager()->GetFont(a_label.font))
    {
        return std::make_shared<og::Entity>(a_label.position.x(), a_label.position.y());
    }

    return nullptr;
}

EntityPtr EntityFactory::CreateCircle(const WOGCircle& aDef)
{
    auto entity = std::make_shared<og::Entity>(aDef.position.x(), -aDef.position.y());
    if (flag & DEBUG)
    {
        auto spr = std::make_shared<CircleSprite>(aDef.radius);
        entity->SetGraphic(spr);
        entity->SetDepth(1000);
    }

    {
        using namespace og::physics;
        PhysicsBodyBuilder builder(GetPhysicsEngine());
        aDef.dynamic ? builder.SetBodyType(BodyDef::e_dynamic) : builder.SetBodyType(BodyDef::e_static);
        builder.SetPosition(aDef.position).SetShapeType(Shape::e_circle).SetRadius(aDef.radius);
        entity->SetPhysicsBody(builder.Build());
    }

    return entity;
}

EntityPtr EntityFactory::CreateRect(const WOGRectangle& aDef)
{
    auto entity = std::make_shared<og::Entity>(0, 0);
    if (!aDef.image.image.isEmpty())
    {
        const auto& image = aDef.image;
        entity->SetPosition(image.imagepos.x(), -image.imagepos.y());
        auto src = GetImageSourceById(image.image);
        auto spr = OGSprite::Create(src);
        spr->CenterOrigin();
        spr->SetAngle(qRadiansToDegrees(-image.imagerot));
        spr->SetScale(image.imagescale);
        entity->SetGraphic(spr);
    }
    else
    {
        entity->SetPosition(aDef.position.x(), -aDef.position.y());
        if (flag & DEBUG)
        {
            auto spr = std::make_shared<RectSprite>(aDef.size);
            spr->SetAngle(qRadiansToDegrees(-aDef.rotation));
            entity->SetGraphic(spr);
            entity->SetDepth(1000);
        }
    }

    {
        using namespace og::physics;
        PhysicsBodyBuilder builder(GetPhysicsEngine());
        aDef.dynamic ? builder.SetBodyType(BodyDef::e_dynamic) : builder.SetBodyType(BodyDef::e_static);
        builder.SetPosition(aDef.position).SetAngle(aDef.rotation).SetShapeType(Shape::e_polygon).SetSize(aDef.size);
        entity->SetPhysicsBody(builder.Build());
    }

    return entity;
}

EntityPtr EntityFactory::CreateLine(const WOGLine& aDef)
{
    const auto MAX_LENGTH = 2000.0f;
    auto normal = aDef.normal * MAX_LENGTH;
    auto x1 = aDef.anchor.x() + -normal.y();
    auto y1 = aDef.anchor.y() + normal.x();
    auto x2 = aDef.anchor.x() + normal.y();
    auto y2 = aDef.anchor.y() + -normal.x();

    auto entity = std::make_shared<og::Entity>(aDef.anchor.x(), -aDef.anchor.y());
    if (flag & DEBUG)
    {
        auto spr = std::make_shared<LineSprite>(QLine(x1, -y1, x2, -y2));
        entity->SetGraphic(spr);
        entity->SetDepth(1000);
    }

    {
        using namespace og::physics;
        PhysicsBodyBuilder builder(GetPhysicsEngine());
        builder.SetBodyType(BodyDef::e_static).SetShapeType(Shape::e_edge).SetLine(x1, y1, x2, y2);
        entity->SetPhysicsBody(builder.Build());
    }

    return entity;
}
