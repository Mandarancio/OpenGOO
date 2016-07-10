#include "gamecontroller.h"
#include "entityfactory.h"
#include "gamedata.h"
#include "og_ipipe.h"
#include "opengoo.h"


GameController::GameController(GameData& a_gdata)
    : Entity(0, 0),
      m_pipe(nullptr),
      m_gameData(a_gdata)
{ 
}

GameController::~GameController()
{
}

void GameController::Added()
{
    if (m_gameData.pipe)
        m_pipe.reset(m_gameData.efactory->CreatePipe(*m_gameData.pipe));

    if (m_gameData.levelexit)
    {
        auto e = m_gameData.efactory->CreateExit(*m_gameData.levelexit, this);
        GetScene()->AddEntity(e);
        m_ballsRequired = m_gameData.ballsrequired;
    }
}

void GameController::Update()
{
}

void GameController::OnOpen()
{  
    if (m_pipe)
        m_pipe->Open();
}

void GameController::OnClosed()
{
    if (m_pipe)
        m_pipe->Close();
}

