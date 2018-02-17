#include "GameEngine/og_gameengine.h"

#include "gamecontroller.h"

GameController::GameController()
    : Entity(0, 0)
{
}

GameController::~GameController()
{
}

void GameController::Added()
{
    if (auto snd = GE->GetResourceManager()->GetMusic(mMusic))
    {
        snd->Play(true);
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
