#pragma once

#include "GameEngine/entity.h"


class QString;
class QPointF;
struct WOGPipe;
class OGIPipe;
struct GameData;
struct WOGLevelExit;
struct ExitEventListener;

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
};

