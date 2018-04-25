#include "GameEngine/og_gameengine.h"

#include "gamecontroller.h"

GameController::GameController()
    : Entity(0, 0)
    , mMusic(nullptr)
{
}

GameController::~GameController()
{
}

void GameController::Added()
{
    if (mMusic)
    {
        mMusic->Play(true);
    }

    foreach (auto& snd, mLoopSounds)
    {
        snd->Play();
    }
}

void GameController::Removed()
{
}

void GameController::Update()
{
}

void GameController::Render(QPainter& /*a_painter*/)
{
}
