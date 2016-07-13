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
    bool m_hasLevelExit;
    int m_balls;
    bool m_isShowProgress;

    void OnOpen();
    void OnClosed();
    void OnBallExit();

    void Update();

    void Added();

    void Render(QPainter& a_painter);

public:
    GameController(GameData& a_gdata);
    ~GameController();
};
