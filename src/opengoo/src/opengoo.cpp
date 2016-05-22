/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "opengoo.h"
#include "og_world.h"
#include "flags.h"
#include "GameEngine/og_gameengine.h"
#include <logger.h>
#include "og_windowcamera.h"
#include "og_ballconfig.h"
#include "og_ball.h"
#include "og_sprite.h"
#include "og_ibody.h"
#include "og_strand.h"
#include "og_button.h"
#include "og_pipe.h"
#include "exit.h"
#include "continuebutton.h"
#include "og_fpscounter.h"
#include "og_forcefield.h"

#include <QMouseEvent>
#include <QTime>
#include <QDebug>

using namespace og;

OpenGOO* OpenGOO::pInstance_ = 0;

OpenGOO* OpenGOO::instance()
{
    if (!pInstance_) pInstance_ = new OpenGOO;

    return pInstance_;
}

void OpenGOO::Destroy()
{
    delete pInstance_;
    pInstance_ = 0;
}

void OpenGOO::AddSprite(float depth, OGSprite* sprite)
{
    if (layers_.contains(depth))
    {
        layers_[depth].Add(sprite);
    }
    else
    {
        OGLayer l;
        l.Add(sprite);
        layers_.insert(depth, l);
    }
}

void OpenGOO::ClearSprites()
{
    _ClearLayers();
}

OGBall* OpenGOO::GetNearestBall() { return pWorld_->nearestball(); }

void OpenGOO::SetLevelName(const QString &levelname)
{
    levelName_ = levelname;
}

void OpenGOO::OpenPipe()
{
    pWorld_->pipe()->Open();
}

void OpenGOO::ClosePipe()
{
    pWorld_->pipe()->Close();
}

void OpenGOO::ShowProgress()
{
    pContinueBtn_.reset();
    pWorld_->exit()->Close();
    _InitProgressWindow();
}

void OpenGOO::SetLanguage(const QString &language) { language_ = language; }

void OpenGOO::_Start()
{
    const int STEPS = 60;
    pCamera_ = 0;
    pGameTime_ = 0;
    _ClearSelectedBall();
    _pFPS = 0;
    SetPause(false);

    //initialize randseed
    qsrand(QTime::currentTime().toString("hhmmsszzz").toUInt());

    pWorld_ = new OGWorld;

    if (language_.isEmpty()) pWorld_->SetLanguage("en");
    else pWorld_->SetLanguage(language_);

    if (!pWorld_->Initialize()) { return; }

    if (levelName_.isEmpty() || (!OGWorld::isExist(levelName_)))
    {
        _LoadMainMenu();
    }
    else _CreateLevel(levelName_);

    if (flag & FPS)
    {
        _pFPS.reset(new OGFPSCounter(QRect(20, 20, 40, 40)));
    }

    width_ = OGGameEngine::getInstance()->getWidth();
    height_ = OGGameEngine::getInstance()->getHeight();

    timeScrollStep_ = width_ / 1000.0f;
    timeStep_ = STEPS / 1000.0f;    
}

void OpenGOO::_End()
{
    _pFPS.reset();
    pContinueBtn_.reset();
    pProgressWnd_.reset();

    _RemoveLevel();
    _RemoveIsland();
    _CloseMainMenu();

    if (pWorld_)
    {        
        logDebug("Clear world");
        delete pWorld_;
    }
}

void OpenGOO::_Activate() { SetPause(false); }
void OpenGOO::_Deactivate() { SetPause(true); }

void OpenGOO::_Cycle()
{
    if (!pWorld_->isLevelLoaded())
    {
        _Quit();

        return;
    }

    if (!pGameTime_)
    {
        lastTime_ = 0;
        pGameTime_ =  new QTime;
        pGameTime_->start();
    }
    else lastTime_ = pGameTime_->restart();

    if (flag & FPS) _pFPS->Update(lastTime_);

    if (pCamera_)
    {
        pCamera_->Update(lastTime_);
        _Scroll();
    }

    if (isPause()) return;

    if (_pSelectedBall && !_pSelectedBall->IsDragging())
    {
        if (!_pSelectedBall->TestPoint(lastMousePos_))
        {
            _pSelectedBall->SetMarked(false);
            _ClearSelectedBall();
        }
    }

    Q_FOREACH(OGBall * ball, pWorld_->balls())
    {
        ball->Update();
    }

    if (pWorld_->exit() && !pProgressWnd_)
    {
        pWorld_->exit()->Update();
    }

    {
        int n = (lastTime_ * timeStep_) + 0.5f; // round

        for (int i = 0; i < n; i++)
        {
            for (unsigned int j=0; j < pWorld_->forcefilds().size(); j++)
            {
                pWorld_->forcefilds()[j]->update();
            }

            pWorld_->Update();
        }
    }

    if (isLevelExit_)
    {
        balls_ = pWorld_->exit()->Balls();

        if (balls_ >= ballsRequired_ && !isContinue_)
        {
            isContinue_ = true;
            _CreateContinueButton();
            pLevel_->hideButton();
        }
    }
}

void OpenGOO::_Paint(QPainter* painter)
{
    painter->setViewport(0, 0, width_, height_);

    if (pWorld_->isLevelLoaded())
    {
        painter->setWindow(pCamera_->rect());

        painter->setRenderHint(QPainter::HighQualityAntialiasing);

        // Paint a scene
        QMutableMapIterator<float, OGLayer> i(layers_);

        while (i.hasNext())
        {
            i.next();
            i.value().Paint(painter);
        }

        Q_FOREACH(OGIBody * body, pWorld_->staticbodies())
        {
            body->Draw(painter);
        }

        Q_FOREACH(OGBall * ball, pWorld_->balls())
        {
            ball->Paint(painter, pWorld_->leveldata()->visualdebug);
        }

        Q_FOREACH(OGStrand * strand, pWorld_->strands())
        {
            strand->Paint(painter, pWorld_->leveldata()->visualdebug);
        }

        if (pWorld_->leveldata() && pWorld_->leveldata()->visualdebug)
        {
            visualDebug(painter, pWorld_, pCamera_->zoom());
        }
    }
}

void OpenGOO::_MouseButtonDown(QMouseEvent* ev)
{
    if (isPause() || !pCamera_ || pProgressWnd_) return;

    QPoint mPos = pCamera_->windowToLogical(ev->pos());

    Q_FOREACH(OGButton * button, pWorld_->buttons())
    {
        if (button->TestPoint(mPos))
        {
            if (button->onclick() == "quit") { _Quit(); }
            else if (button->onclick() == "credits") { }
            else if (button->onclick() == "showselectprofile") { }
            else if (button->onclick() == "island1")
            {
                _SetIsland("island1"); // Saves the name of island
                LoadIsland(_GetIsland());
            }
            else if (button->onclick() == "island2")
            {
                _SetIsland("island2"); // Saves the name of island
                LoadIsland(_GetIsland());
            }
            else if (button->onclick() == "island3")
            {
                _SetIsland("island3"); // Saves the name of island
                LoadIsland(_GetIsland());
            }
            else if (button->onclick() == "island4")
            {
                _SetIsland("island4"); // Saves the name of island
                LoadIsland(_GetIsland());
            }
            else if (button->onclick() == "island5")
            {
                _SetIsland("island5"); // Saves the name of island
                LoadIsland(_GetIsland());
            }
            else if (!button->onclick().isEmpty())
            {
                QString name = button->getLevelName();

                if (!name.isEmpty())
                {
                    loadLevel(name);
                }
            }

            break;
        }
    }    

    if (_pSelectedBall && _pSelectedBall->IsDraggable())
    {
        _pSelectedBall->MouseDown(mPos);
    }
}

void OpenGOO::_MouseButtonUp(QMouseEvent* ev)
{
    if (isPause() || !pCamera_ || pProgressWnd_) return;

    if (_pSelectedBall && _pSelectedBall->IsDragging() && pCamera_)
    {
        _pSelectedBall->MouseUp(pCamera_->windowToLogical(ev->pos()));
    }
}

void OpenGOO::_MouseMove(QMouseEvent* ev)
{
    if (isPause() || !pCamera_ || pProgressWnd_) return;

    curMousePos_ = ev->pos();

    QPoint mPos(ev->pos());

    mPos = pCamera_->windowToLogical(mPos);
    lastMousePos_ = mPos;

    Q_FOREACH(OGButton * button, pWorld_->buttons())
    {
        if (button->TestPoint(mPos))
        {
            button->up()->visible = false;
            button->over()->visible = true;
        }
        else
        {
            button->up()->visible = true;
            button->over()->visible = false;
        }
    }

    if (!_pSelectedBall)
    {
        Q_FOREACH(OGBall * ball, pWorld_->balls())
        {
            if (ball->TestPoint(mPos))
            {
                _pSelectedBall = ball;
                _pSelectedBall->SetMarked(true);
                break;
            }
        }
    }
    else if (_pSelectedBall->IsDragging())
    {
        _pSelectedBall->MouseMove(mPos);
    }
    else if (!_pSelectedBall->TestPoint(mPos))
    {
        _pSelectedBall->SetMarked(false);
        _ClearSelectedBall();
    }
}

void OpenGOO::_KeyDown(QKeyEvent* ev)
{
    Q_UNUSED(ev)
}

inline void OpenGOO::_ClearLayers()
{
    layers_.clear();
}

inline void OpenGOO::_Quit() { OGGameEngine::getInstance()->quit(); }

void OpenGOO::_Scroll()
{
    if (isPause() || pProgressWnd_) return;

    float OFFSET = 50.0f;
    float sx = curMousePos_.x();
    float sy = curMousePos_.y();
    int shift = lastTime_;

    if (sx <= OFFSET) { pCamera_->ScrollLeft(shift); }
    else if (sx >= width_ - OFFSET) { pCamera_->ScrollRight(shift); }

    if (sy <= OFFSET) { pCamera_->ScrollUp(shift); }
    else if (sy >= height_ - OFFSET) { pCamera_->ScrollDown(shift);}
}

void OpenGOO::_SetDebug(bool debug)
{
    Q_FOREACH(OGIBody * body, pWorld_->staticbodies())
    {
        body->SetDebug(debug);
    }
}

void OpenGOO::_SetBackgroundColor(const QColor &color)
{
    OGGameEngine::getInstance()->getWindow()->setBackgroundColor(color, true);
}

void OpenGOO::_CreateContinueButton()
{
    pContinueBtn_.reset(new ContinueButton);
    auto btn = pContinueBtn_.get();
    int x = width_ - (btn->width() + 20);
    int y = 20;
    btn->setPosition(x, y);
    connect(btn, SIGNAL(pressed()), this, SLOT(_closeContinueButton()));
    btn->setVisible(true);
}

void OpenGOO::_InitProgressWindow()
{
    pProgressWnd_.reset(new ProgressWindow);
    auto wnd = pProgressWnd_.get();
    connect(wnd, SIGNAL(close()), this, SLOT(_closeProgressWindow()));
    pProgressWnd_->setBalls(balls_, balls_ - ballsRequired_);
}

// Level
void OpenGOO::_LoadLevel(const QString &levelname)
{
    if (!pWorld_->LoadLevel(levelname)) return;
    if (pWorld_->leveldata()->visualdebug) _SetDebug(true);

    _SetBackgroundColor(pWorld_->scenedata()->backgroundcolor);

    pCamera_ = OGWindowCamera::instance();
    SetPause(false);

    if (pWorld_->exit())
    {
        isLevelExit_ = true;
        balls_ = 0;
        ballsRequired_ = pWorld_->leveldata()->ballsrequired;
        isContinue_ = false;
    }
    else { isLevelExit_ = false; }
}

void OpenGOO::_CloseLevel()
{
    pWorld_->CloseLevel();
    _ClearLayers();
    pCamera_ = 0;
}

void OpenGOO::ReloadLevel()
{
    pWorld_->Reload();
    pCamera_->SetLastPosition();
    balls_ = 0;
}
void OpenGOO::_CreateLevel(const QString &levelname)
{
    pLevel_.reset(new Level(levelname));
    auto lvl = pLevel_.get();
    connect(lvl, SIGNAL(closeLevel()), this, SLOT(_backToIsland()));
}

void OpenGOO::_RemoveLevel() { pLevel_.reset(); }

// Main menu
inline QString OpenGOO::_GetMainMenu() { return "MapWorldView"; }

inline void OpenGOO::_LoadMainMenu() { _LoadLevel(_GetMainMenu()); }

inline void OpenGOO::_CloseMainMenu() { _CloseLevel(); }

// Island public interface
void OpenGOO::LoadIsland(const QString &name)
{
    _CloseMainMenu();
    _CreateIsland(name);
}

// Island private interface
inline QString OpenGOO::_GetIsland() { return _currentIsland; }
inline void OpenGOO::_SetIsland(const QString &name) { _currentIsland = name; }

inline void OpenGOO::_CreateIsland(const QString &name)
{   
    pIsland.reset(new Island(name));
    auto island = pIsland.get();
    connect(island, SIGNAL(close()), this, SLOT(_backToMainMenu()));
}

inline void OpenGOO::_RemoveIsland()
{
    pIsland.reset();
}

// Slots
void OpenGOO::_backToMainMenu()
{
    _RemoveIsland();
    _LoadMainMenu();
}

void OpenGOO::_backToIsland()
{
    _RemoveLevel();
    _CreateIsland(_GetIsland());
}

void OpenGOO::loadLevel(const QString &levelname)
{
     _RemoveIsland();
     _CreateLevel(levelname);
}

void OpenGOO::_closeContinueButton()
{
    pContinueBtn_.reset();    
    _InitProgressWindow();
}

void OpenGOO::_closeProgressWindow()
{
    pProgressWnd_.reset();
    _backToIsland();
}
