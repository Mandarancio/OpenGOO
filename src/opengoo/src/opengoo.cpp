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
#include "og_fpscounter.h"
#include "flags.h"
#include "GameEngine/og_gameengine.h"
#include <logger.h>
#include "og_event.h"
#include "og_windowcamera.h"
#include "og_ballconfig.h"
#include "og_uiscene.h"

#include <QTime>

void visualDebug(QPainter* painter, OGWorld* world, qreal zoom);

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

OGBall* OpenGOO::GetNearestBall() { return pWorld_->nearestball(); }

void OpenGOO::SetLevelName(const QString& levelname)
{
    levelName_ = levelname;
}

void OpenGOO::SetLanguage(const QString& language) { language_ = language; }

void OpenGOO::SendEvent(OGEvent* ev) { pInstance_->eventList_ << ev; }

void OpenGOO::_Start()
{
    const int STEPS = 60;
    pCamera_ = 0;
    pGameTime_ = 0;
    pSelectedBall_ = 0;
    pFPS_ = 0;
    isPause_ = false;

    //initialize randseed
    qsrand(QTime::currentTime().toString("hhmmsszzz").toUInt());

    pWorld_ = new OGWorld;

    if (language_.isEmpty()) { language_ = "en"; }

    pWorld_->SetLanguage(language_);

    if (!pWorld_->Initialize()) { return; }    

    if (levelName_.isEmpty() || (!OGWorld::isExist(levelName_)))
    {        
        _LoadMainMenu();
    }
    else
    {
        _LoadLevel(levelName_);
        _CreateUIButtons();
    }

    if (flag & FPS) { pFPS_ = new OGFPSCounter; }

    width_ = OGGameEngine::getEngine()->getWidth();
    height_ = OGGameEngine::getEngine()->getHeight();

    timeScrollStep_ = width_ / 1000.0f;
    timeStep_ = STEPS / 1000.0f;
}

void OpenGOO::_End()
{
    _ClearUI();
    delete pFPS_;

    if (pWorld_)
    {
        logDebug("Clear world");
        delete pWorld_;
    }

}

void OpenGOO::_Activate() { isPause_ = false; }
void OpenGOO::_Deactivate() { isPause_ = true; }

void OpenGOO::_Cycle()
{
    if (!eventList_.isEmpty())
    {
        OGEvent* e = eventList_.takeFirst();

        switch (e->type())
        {
            case OGEvent::EXIT:
                _HQuit();
                break;

            case OGEvent::CREATE_MENU:
                _HCreateMenu(e);
                break;

            case OGEvent::RESTART:
                _HRestart();
                break;

            case OGEvent::SHOW_OCD:
                _HShowOCD();
                break;

            case OGEvent::BACKTO_ISLAND:
                _HBackToIsland();
                break;

            case OGEvent::RESUME:
                _HResume();
                break;

            case OGEvent::BACKTO_MAINMENU:
                _HBackToMainMenu();
                break;

            case OGEvent::LOAD_ISLAND:
                _HLoadIsland(e);
                break;

            case OGEvent::LOAD_LEVEL:
                _HLoadLevel(e);
                break;

            default:
                break;
        }

        delete e;
    }

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

    if (flag & FPS) pFPS_->Update(lastTime_);

    if (!isPause_)
    {
        int n = (lastTime_ * timeStep_) + 0.5f; // round

        for (int i = 0; i < n; i++) { pWorld_->Update(); }
    }

    if (pCamera_)
    {
        pCamera_->Update(lastTime_);
        _Scroll();
    }

    if (pSelectedBall_ && !pSelectedBall_->IsDragging())
    {
        if (!pSelectedBall_->TestPoint(lastMousePos_))
        {
            pSelectedBall_->SetMarked(false);
            pSelectedBall_ = 0;
        }
    }

    Q_FOREACH(OGBall * ball, pWorld_->balls())
    {
        ball->Update();
    }
}

void OpenGOO::_Paint(QPainter *painter)
{
    painter->setViewport(0, 0, width_, height_);

    if (pWorld_->isLevelLoaded())
    {
        _SetBackgroundColor(pWorld_->scenedata()->backgroundcolor);

        painter->setWindow(pCamera_->rect());

        painter->setRenderHint(QPainter::HighQualityAntialiasing);

        // Paint a scene
        Q_FOREACH(OGSprite * sprite, pWorld_->sprites())
        {
            sprite->Paint(painter);
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

    painter->setWindow(0, 0, width_, height_);

    if (flag & FPS) pFPS_->Painter(painter);

    Q_FOREACH(OGUI * ui, uiList_) { ui->Paint(painter); }
}

void OpenGOO::_MouseButtonDown(QMouseEvent *ev)
{
    Q_FOREACH(OGUI * ui, uiList_) { if (ui != 0) ui->_MouseDown(ev); }

    if (isPause_ || !pCamera_) return;

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
                SendEvent(new OGIslandEvent("island1"));
            }
            else if (!button->onclick().isEmpty())
            {
                QString name = button->getLevelName();

                if (!name.isEmpty())
                {
                    SendEvent(new OGLevelEvent(name));
                }
            }
        }
    }

    if (pSelectedBall_ && pSelectedBall_->IsDraggable())
    {
        pSelectedBall_->MouseDown(mPos);
    }
}

void OpenGOO::_MouseButtonUp(QMouseEvent *ev)
{
    if (pSelectedBall_ && pSelectedBall_->IsDragging() && pCamera_)
    {
        pSelectedBall_->MouseUp(pCamera_->windowToLogical(ev->pos()));
    }
}

void OpenGOO::_MouseMove(QMouseEvent *ev)
{
    curMousePos_ = ev->pos();

    QPoint mPos(ev->pos());

    Q_FOREACH(OGUI * ui, uiList_)
    {
        if (ui != 0) ui->_MouseMove(ev);
    }

    if (isPause_ || !pCamera_) return;

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

    if (!pSelectedBall_)
    {
        Q_FOREACH(OGBall * ball, pWorld_->balls())
        {
            if (ball->TestPoint(mPos))
            {
                pSelectedBall_ = ball;
                pSelectedBall_->SetMarked(true);
                break;
            }
        }
    }
    else if (pSelectedBall_->IsDragging())
    {
        pSelectedBall_->MouseMove(mPos);
    }
    else if (!pSelectedBall_->TestPoint(mPos))
    {
        pSelectedBall_->SetMarked(false);
        pSelectedBall_ = 0;
    }
}

void OpenGOO::_Quit() { SendEvent(new OGEvent(OGEvent::EXIT)); }

void OpenGOO::_Scroll()
{
    if (isPause_) return;

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

inline void OpenGOO::_SetBackgroundColor(const QColor &color)
{
    glClearColor(color.redF(), color.greenF(), color.blueF(), 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

// Level
void OpenGOO::_LoadLevel(const QString& levelname)
{
    pWorld_->SetLevelname(levelname);

    if (!pWorld_->Load()) { return; }

    pWorld_->CreateScene();

    if (!pWorld_->isLevelLoaded())
    {
        _CloseLevel();

        return;
    }

    if (pWorld_->leveldata()->visualdebug) _SetDebug(true);

    pCamera_ = OGWindowCamera::instance();

    isPause_ = false;
}

void OpenGOO::_CloseLevel()
{
    pWorld_->CloseLevel();
    pCamera_ = 0;
}

void OpenGOO::_ReloadLevel()
{
    pWorld_->Reload();
    pCamera_->SetLastPosition();
}

// User interface

void OpenGOO::_CreateUI(const QString  &name)
{
    if (!uiList_.contains(name))
    {
        OGUI* ui = OGUIScene::CreateUI(name);

        if (ui != 0) uiList_.insert(name, ui);
    }
}

void OpenGOO::_ClearUI()
{
    Q_FOREACH(OGUI * ui, uiList_) { delete ui; }

    uiList_.clear();
}

void OpenGOO::_CreateUIBack() { _CreateUI("uiBack"); }
void OpenGOO::_CreateUIButtons() {  _CreateUI("ui"); }

// Menus

void OpenGOO::_CreateMenu(const QString &name)
{
    isPause_ = true;
    _ClearUI();
    _CreateUI(name);
}

QString OpenGOO::_GetMainMenu() { return "MapWorldView"; }

void OpenGOO::_LoadMainMenu()
{
    _CloseLevel();
    _ClearUI();

    _LoadLevel(_GetMainMenu());
}

// Islands
QString OpenGOO::_GetIsland() { return island_; }
void OpenGOO::_SetIsland(const QString &name) { island_ = name; }

void OpenGOO::_LoadIsland(const QString &name)
{
    _CloseLevel();
    _ClearUI();

    _LoadLevel(name);
    _CreateUIBack();
}

// Handlers

void OpenGOO::_HQuit()
{
    OGGameEngine::getEngine()->getWindow()->close();
}


void OpenGOO::_HCreateMenu(OGEvent* ev)
{
    _ClearUI();
    _CreateMenu(ev->args()->first());
}

// Handler of a back button;
void OpenGOO::_HBackToMainMenu()
{
    _LoadMainMenu();
}

void OpenGOO::_HLoadLevel(OGEvent* ev)
{
    _CloseLevel();
    _ClearUI();

    _LoadLevel(ev->args()->first());
    _CreateUIButtons();
}

void OpenGOO::_HLoadIsland(OGEvent* ev)
{
    QString island = ev->args()->first();
    _LoadIsland(island);
    _SetIsland(island);
}

// Game menu
void OpenGOO::_HRestart()
{
    _ClearUI();
    _ReloadLevel();
    _CreateUIButtons();
    isPause_ = false;
}

void OpenGOO::_HShowOCD() {}

void OpenGOO::_HBackToIsland()
{
    QString name = _GetIsland();

    if (name.isEmpty()) _Quit();
    else _LoadIsland(name);

    isPause_ = false;
}

void OpenGOO::_HResume()
{
    _ClearUI();
    _CreateUIButtons();
    isPause_ = false;
}
