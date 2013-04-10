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

#include <QTime>

OpenGOO* OpenGOO::instance_ = 0;

OpenGOO* OpenGOO::instance()
{
    if (!instance_) instance_ = new OpenGOO;

    return instance_;
}

void OpenGOO::Destroy()
{
    delete instance_;
    instance_ = 0;
}

void OpenGOO::SetLevelName(const QString& levelname)
{
    levelName_ = levelname;
}

void OpenGOO::SetLanguage(const QString& language)
{
    language_ = language;
}

void OpenGOO::_Start()
{
    const int STEPS = 60;

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

    if (flag & FPS)
    {
        pFPS_ = new OGFPSCounter;
    }

    width_ = OGGameEngine::getEngine()->getWidth();
    height_ = OGGameEngine::getEngine()->getHeight();

    timeScrollStep_ = width_ / 1000;
    timeStep_ = STEPS / 1000;
}

void OpenGOO::_End()
{

}

void OpenGOO::_LoadLevel(const QString& levelname)
{

}

void OpenGOO::_CreateUIButtons()
{

}

void OpenGOO::_LoadMainMenu()
{

}

#include "og_gameengine.h"
#include "og_world.h"
#include "og_windowcamera.h"
#include "og_uiscene.h"
#include "og_event.h"
#include "OGLib/icamera.h"

#include <QDir>
#include <QTimer>

#include <memory>

#include "OGLib/point.h"

#include <QRect>
#include <QDebug>
#include <QTime>

#include <logger.h>

#include "og_ballconfig.h"

const QString MAIN_MENU = "MapWorldView";
const float K = 10.0f;
const int FRAMERATE = 60;
const int STEPS = 60;

OGWorld* _world;

std::unique_ptr<QTime> _gameTime;
int _lastTime;
float _timeStep;
float _timeScrollStep;

QHash<QString, OGUI*> _listUI;

OGBall* _selectedBall = 0;

QString _levelname;
QString _island;

bool _isPause;

QList<OGEvent*> _eventList;

int _scrolltime = 0;
std::auto_ptr<OGWindowCamera> _camera;
int _width;
int _height;

class OGFPSCounter* _pFPS = 0;

QPoint _lastMousePos;
QPoint _curMousePos;

// Default settings
OGConfig _config = {
    800,    // width
    600,    // height
    true,   // fullscreen
    "en"      // language
};

void closeGame();

void mouseEven(OGUI* ui,QMouseEvent* e);

void createUI(const QString& name);
void createUIButtons();
void createUIBack();
void clearUI();

void createMenu(const QString &name);

void backToIsland();
void restartLevel();
void resumeGame();
void showOCDCriterial();

void loadLevel(const QString& name);
void reloadLevel();
void closeLevel();

QString getIsland();
void setIsland(const QString& name);
void loadIsland(const QString& name);

void loadMainMenu();

void scroll();
void visualDebug(QPainter* painter, OGWorld* world, qreal zoom);

QPointF logicalToWindow(const QRectF & rect, qreal zoom);

void setBackgroundColor(const QColor & color);
void drawOpenGLScene();
void draw(QPainter* p, OGWorld* w);


void setDebug(bool debug);

#define UNIMPLEMENTED qWarning() << __FUNCTION__ << "is UNIMPLEMENTED!";

void GameStart()
{
    //initialize randseed
    qsrand(QTime::currentTime().toString("hhmmsszzz").toUInt());

    _timeScrollStep = _width * 0.001f;

    _world = new OGWorld;

    if (_config.language.isEmpty()) { _config.language = "en"; }

    _world->SetLanguage(_config.language);

    if (!_world->Initialize()) { return; }

    if (_levelname.isEmpty() || (!OGWorld::isExist(_levelname)))
    {
        loadMainMenu();
    }
    else
    {
        loadLevel(_levelname);
        createUIButtons();
    }

    _timeStep = STEPS * 0.001f;   

    if (flag & FPS)
    {
        _pFPS = new OGFPSCounter;
    }

    _width = OGGameEngine::getEngine()->getWidth();
    _height = OGGameEngine::getEngine()->getHeight();
}

void GameEnd()
{
    clearUI();

    delete _pFPS;

//    delete _gameEngine;

    if (_world)
    {
        logDebug("Clear world");

        delete _world;
    }
}

void GameCycle()
{
    if (!_eventList.isEmpty())
    {
        OGEvent* e = _eventList.takeFirst();

        switch (e->type())
        {
            case OGEvent::EXIT:
                closeGame();
                break;

            case OGEvent::CREATE_MENU:
                createMenu(e->args()->first());
                break;

            case OGEvent::RESTART:
                restartLevel();
                break;

            case OGEvent::SHOW_OCD:
                showOCDCriterial();
                break;

            case OGEvent::BACKTO_ISLAND:
                backToIsland();
                break;

            case OGEvent::RESUME:
                resumeGame();
                break;

            case OGEvent::BACKTO_MAINMENU:
                loadMainMenu();
                break;

            case OGEvent::LOAD_ISLAND:
                setIsland(e->args()->first());
                loadIsland(e->args()->first());
                break;

            case OGEvent::LOAD_LEVEL:
                clearUI();
                closeLevel();
                loadLevel(e->args()->first());
                createUIButtons();
                break;

            default:
                break;
        }

        delete e;
    }

    if (!_world->isLevelLoaded())
    {
        closeGame();

        return;
    }

    if (!_gameTime)
    {
        _lastTime = 0;
        _gameTime = std::unique_ptr<QTime>(new QTime);
        _gameTime->start();
    }
    else _lastTime = _gameTime->restart();

    if (flag & FPS) _pFPS->Update(_lastTime);

    if (!_isPause)
    {
        int n = qRound(_lastTime * _timeStep);

        for (int i = 0; i < n; i++) { _world->Update(); }
    }

    if (_camera.get() != 0)
    {
        _camera->Update(_lastTime);
        scroll();
    }

    if (_selectedBall != 0 && !_selectedBall->IsDragging())
    {
        if (!_selectedBall->TestPoint(_lastMousePos))
        {
            _selectedBall->SetMarked(false);
            _selectedBall = 0;
        }
    }

    Q_FOREACH(OGBall * ball, _world->balls())
    {
        ball->Update();
    }    
}

void GamePaint(QPainter* painter)
{
    if (!_world->isLevelLoaded()) { return; }

    painter->setWindow(_camera->rect());
    painter->setViewport(0, 0, _width, _height);

    painter->setRenderHint(QPainter::HighQualityAntialiasing);

    draw(painter, _world);

    if (_world->leveldata() != 0 && _world->leveldata()->visualdebug)
    {
        visualDebug(painter, _world, _camera->zoom());
    }

    painter->setWindow(0, 0, _width, _height);

    if (flag & FPS) _pFPS->Painter(painter);

    Q_FOREACH(OGUI * ui, _listUI)
    {
        ui->Paint(painter);
    }
}

void GameActivate() { _isPause = false; }
void GameDeactivate() { _isPause = true; }

void KeyDown(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Escape:
            break;
    }
}

void KeyUp(QKeyEvent* event) { Q_UNUSED(event) }

void MouseButtonDown(QMouseEvent* event)
{
    Q_FOREACH(OGUI * ui, _listUI)
    {
        if (ui != 0) mouseEven(ui, event);
    }

    if (_isPause || _camera.get() == 0) return;

    QPoint mPos = _camera->windowToLogical(event->pos());

    Q_FOREACH(OGButton * button, _world->buttons())
    {
        if (button->TestPoint(mPos))
        {
            if (button->onclick() == "quit")
            {
                SendEvent(new OGEvent(OGEvent::EXIT));
            }
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

    if (_selectedBall != 0 && _selectedBall->IsDraggable())
    {
        _selectedBall->MouseDown(mPos);
    }
}

void MouseButtonUp(QMouseEvent* event)
{
    if (_selectedBall != 0 && _selectedBall->IsDragging() && _camera.get() != 0)
    {
        _selectedBall->MouseUp(_camera->windowToLogical(event->pos()));
    }
}

void MouseMove(QMouseEvent* event)
{
    _curMousePos = event->pos();

    QPoint mPos(event->pos());

    Q_FOREACH(OGUI * ui, _listUI)
    {
        if (ui != 0) mouseEven(ui, event);
    }

    if (_isPause || _camera.get() == 0) return;

    mPos = _camera->windowToLogical(mPos);
    _lastMousePos = mPos;

    Q_FOREACH(OGButton * _button, _world->buttons())
    {
        if (_button->TestPoint(mPos))
        {
            _button->up()->visible = false;
            _button->over()->visible = true;
        }
        else
        {
            _button->up()->visible = true;
            _button->over()->visible = false;
        }
    }

    if (_selectedBall == 0)
    {
        Q_FOREACH(OGBall * ball, _world->balls())
        {
            if (ball->TestPoint(mPos))
            {
                _selectedBall = ball;
                _selectedBall->SetMarked(true);
                break;
            }
        }
    }
    else if (_selectedBall->IsDragging())
    {
        _selectedBall->MouseMove(mPos);
    }
    else if (!_selectedBall->TestPoint(mPos))
    {
        _selectedBall->SetMarked(false);
        _selectedBall = 0;
    }
}

void MouseWheel(QWheelEvent* event)
{
    Q_UNUSED(event)
}

void mouseEven(OGUI* ui, QMouseEvent* e) { ui->_MouseEvent(e); }

void scroll()
{
    if (_isPause) return;

    float OFFSET = 50.0f;
    float sx = _curMousePos.x();
    float sy = _curMousePos.y();
    int shift = _lastTime;

    if (sx <= OFFSET) { _camera->ScrollLeft(shift); }
    else if (sx >= _width - OFFSET) { _camera->ScrollRight(shift); }

    if (sy <= OFFSET) { _camera->ScrollUp(shift); }
    else if (sy >= _height - OFFSET) { _camera->ScrollDown(shift);}
}

void SendEvent(OGEvent* ev)
{
    _eventList << ev;
}

QPointF logicalToWindow(const QRectF &rect, qreal zoom)
{
    qreal w = rect.width() / zoom;
    qreal h = rect.height() / zoom;
    qreal x = rect.x() - w * 0.5f;
    qreal y = (rect.y() + h * 0.5f) * (-1.0f);

    return QPointF(x, y);
}

void closeGame()
{
    OGGameEngine::getEngine()->getWindow()->close();
}

void setBackgroundColor(const QColor &color)
{
    glClearColor(color.redF(), color.greenF(), color.blueF(), 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void drawOpenGLScene() {}

void draw(QPainter* p, OGWorld* w)
{
    if (_world->scenedata() != 0)
    {
        setBackgroundColor(_world->scenedata()->backgroundcolor);
    }

    drawOpenGLScene();

    // Paint a scene
    Q_FOREACH(OGSprite * sprite, w->sprites())
    {
        sprite->Paint(p);
    }

    Q_FOREACH(OGIBody * body, w->staticbodies())
    {
        body->Draw(p);
    }

    Q_FOREACH(OGBall * ball, w->balls())
    {
        ball->Paint(p, _world->leveldata()->visualdebug);
    }

    Q_FOREACH(OGStrand * strand, w->strands())
    {
        strand->Paint(p, _world->leveldata()->visualdebug);
    }
}

void createUI(const QString  &name)
{
    if (!_listUI.contains(name))
    {
        OGUI* ui = OGUIScene::CreateUI(name);

        if (ui != 0) _listUI.insert(name, ui);
    }
}

void clearUI()
{
    Q_FOREACH(OGUI * ui, _listUI)
    {
        delete ui;
    }

    _listUI.clear();
}

void createUIButtons() { createUI("ui"); }
void createUIBack() { createUI("uiBack"); }

void createMenu(const QString &name)
{
    _isPause = true;
    clearUI();
    createUI(name);
}

void loadLevel(const QString &name)
{
    _world->SetLevelname(name);

    if (!_world->Load()) { return; }

    _world->CreateScene();

    if (!_world->isLevelLoaded())
    {
        closeLevel();
        return;
    }

    if (_world->leveldata()->visualdebug) setDebug(true);

    _isPause = false;
}

void closeLevel() { _world->CloseLevel(); }

void reloadLevel()
{
    _world->Reload();
    _camera->SetLastPosition();
}

void loadIsland(const QString &name)
{
    clearUI();
    closeLevel();
    loadLevel(name);
    createUIBack();
}

void loadMainMenu()
{
    clearUI();
    closeLevel();
    loadLevel(MAIN_MENU);
}

QString getIsland() { return _island; }
void setIsland(const QString &name) { _island = name; }

// Actions of a menu

void restartLevel()
{
    clearUI();
    reloadLevel();
    createUIButtons();
    _isPause = false;
}

void showOCDCriterial() { UNIMPLEMENTED }

void backToIsland()
{
    QString name = getIsland();

    if (name.isEmpty()) loadMainMenu();
    else loadIsland(name);

    _isPause = false;
}

void resumeGame()
{
    clearUI();
    createUIButtons();
    _isPause = false;
}

void setDebug(bool debug)
{
    Q_FOREACH(OGIBody * body, _world->staticbodies())
    {
        body->SetDebug(debug);
    }
}
