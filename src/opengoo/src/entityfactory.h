#pragma once

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

class OGIPipe;
class OGBall;

class EntityFactory
{
public:
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
};

