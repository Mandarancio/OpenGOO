#pragma once

#include <cassert>

#include "GameEngine/entity.h"


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

class OGIPipe;
class OGBall;

namespace og
{
class OGResourceManager;

namespace physics
{
class PhysicsEngine;
}
}

class EntityFactory
{
    og::OGResourceManager& m_resourceManager;
    og::physics::PhysicsEngine* m_physicEngine;

    og::OGResourceManager& GetResourceManager() const
    {
        return m_resourceManager;
    }

    og::physics::PhysicsEngine& GetPhysicsEngine()
    {
        assert(m_physicEngine);
        return *m_physicEngine;
    }

public:
    EntityFactory(og::OGResourceManager& a_rm)
        : m_resourceManager(a_rm)
        , m_physicEngine(nullptr)
    {
    }

    void SetPhysicsEngine(og::physics::PhysicsEngine* a_physicEngine)
    {
        m_physicEngine = a_physicEngine;
    }

    EntityPtr CreateBend(const QString& a_type,
                         const QPointF& a_p1,
                         const QPointF& a_p2,
                         const QPointF& a_p3,
                         float a_depth);

    EntityPtr CreateCap(const WOGPipe& a_pipe, const QString& a_type);

    OGIPipe* CreatePipe(const WOGPipe& a_pipe);

    EntityPtr CreateGameController(GameData& a_gdata);

    EntityPtr CreateExit(const WOGLevelExit& a_exit, ExitEventListener* a_listener);

    EntityPtr CreateRadialForceField(const WOGRadialForceField& a_field);

    EntityPtr CreateBall(const WOGBallInstance& a_ball);

    EntityPtr CreateStrand(EntityPtr a_ball1, EntityPtr a_ball2);

    EntityPtr CreateButton(const WOGButton& a_btnDef);
};

