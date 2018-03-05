#pragma once

#include <cassert>

#include "GameEngine/entity.h"
#include "entities/ball.h"
#include "wog_ball.h"

class QString;
class QPointF;

struct GameData;
struct ExitEventListener;

struct WOGPipe;
struct WOGLevelExit;
struct WOGRadialForceField;
struct WOGBallInstance;
struct WOGStrand;
struct WOGButton;
struct WOGLabel;
struct WOGCircle;
struct WOGRectangle;
struct WOGLine;

class OGBall;

namespace og
{
struct IResourceManager;

namespace physics
{
class PhysicsEngine;
}
}

class EntityFactory
{
    og::physics::PhysicsEngine* m_physicEngine;

    og::physics::PhysicsEngine* GetPhysicsEngine()
    {
        assert(m_physicEngine);
        return m_physicEngine;
    }

public:
    EntityFactory()
        : m_physicEngine(nullptr)
    {
    }

    void SetPhysicsEngine(og::physics::PhysicsEngine* a_physicEngine)
    {
        m_physicEngine = a_physicEngine;
    }

    EntityPtr CreateCap(const WOGPipe& a_pipe, const QString& a_type);

    EntityPtr CreatePipe(const WOGPipe& a_pipe);

    EntityPtr CreateGameController();

    EntityPtr CreateExit(const WOGLevelExit& a_exit, ExitEventListener* a_listener);

    EntityPtr CreateRadialForceField(const WOGRadialForceField& a_field);

    std::shared_ptr<Ball> CreateBall(const WOGBallInstance& a_ball);

    EntityPtr CreateStrand(EntityPtr a_ball1, EntityPtr a_ball2);

    EntityPtr CreateStrand(EntityPtr aEntity1, EntityPtr aEntity2, const WOGBallStrand& aDef);

    EntityPtr CreateButton(const WOGButton& a_btnDef);

    EntityPtr CreateLabel(const WOGLabel& a_label);

    EntityPtr CreateCircle(const WOGCircle& aDef);

    EntityPtr CreateRect(const WOGRectangle &aDef);

    EntityPtr CreateLine(const WOGLine &aDef);
};
