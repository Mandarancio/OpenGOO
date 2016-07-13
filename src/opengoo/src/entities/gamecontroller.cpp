#include "gamecontroller.h"
#include "entityfactory.h"
#include "gamedata.h"
#include "og_ipipe.h"
#include "opengoo.h"


GameController::GameController(GameData& a_gdata)
    : Entity(0, 0),
      m_gameData(a_gdata)
{ 
    m_ballsRequired = 0;
    m_balls = 0;
    m_hasLevelExit = false;
    m_isShowProgress = false;
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
        m_hasLevelExit = true;
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

void GameController::OnBallExit()
{
    ++m_balls;

    if (m_ballsRequired > 0 && m_balls == m_ballsRequired)
        m_isShowProgress = true;
}

void GameController::Render(QPainter& a_painter)
{
    if (m_hasLevelExit)
    {
        a_painter.save();
        a_painter.resetTransform();
        QFont font("Times", 14, QFont::Bold);
        a_painter.setPen(Qt::white);
        a_painter.setFont(font);
        a_painter.drawText(10, 20, QString::number(m_balls));

        if (m_isShowProgress)
        {
            QString text("Congratulations");
            QFont font("Times", 24, QFont::Bold);
            a_painter.setFont(font);
            auto&& fm = a_painter.fontMetrics();
            auto xx = 400 - fm.width(text) / 2;
            auto yy = 300 - fm.height() / 2;
            a_painter.drawText(xx, yy, text);
        }

        a_painter.restore();
    }
}


