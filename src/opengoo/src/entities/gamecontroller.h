#pragma once

#include "GameEngine/entity.h"
#include "exiteventlistener.h"
#include "gamedata.h"

class OGIPipe;

class GameController : public og::Entity, public ExitEventListener
{
    std::unique_ptr<OGIPipe> m_pipe;
    int m_ballsRequired;
    GameData m_gameData;

    void OnOpen();
    void OnClosed();

    void Update();

    void Added();

public:
    GameController(GameData& a_gdata);
    ~GameController();
};
