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
#include "OGLib/point.h"

#include <QRect>
#include <QDebug>
#include <QTime>

#ifndef Q_OS_WIN32
#include "backtracer.h"
#endif
#ifdef Q_OS_WIN32
#include "backtracer_win32.h"
#endif

#include "flags.h"
#include <logger.h>
#include <consoleappender.h>
#include "og_ballconfig.h"

bool GameInitialize(int argc, char** argv)
{
#ifndef Q_OS_WIN32
    BackTracer(SIGSEGV);
    BackTracer(SIGFPE);
#endif
#ifdef Q_OS_WIN32
    AddVectoredExceptionHandler(0, UnhandledException2);
#endif

    ConsoleAppender* con_apd;
    con_apd  = new ConsoleAppender(LoggerEngine::LevelInfo, stdout, "%d - <%l> - %m%n");
    LoggerEngine::addAppender(con_apd);
    con_apd  = new ConsoleAppender(LoggerEngine::LevelDebug     |
                                   LoggerEngine::LevelWarn      |
                                   LoggerEngine::LevelCritical  |
                                   LoggerEngine::LevelError     |
                                   LoggerEngine::LevelException |
                                   LoggerEngine::LevelFatal, stdout, "%d - <%l> - %m [%f:%i]%n");
    LoggerEngine::addAppender(con_apd);
    qInstallMessageHandler(gooMessageHandler);

    //Check for the run parameters
    for (int i = 1; i < argc; i++)
    {
        QString arg(argv[i]);
        //Check for debug Option
        if (!arg.compare("--debug", Qt::CaseInsensitive))
        {
            flag |= DEBUG;
            logWarn("DEBUG MODE ON");
        }
        if (!arg.compare("--fps", Qt::CaseInsensitive)) { flag |= FPS; }
        else if (!arg.compare("--level", Qt::CaseInsensitive))
        {
            if (++i < argc) { _levelname = QString(argv[i]); }
        }
    }

    //CHECK FOR GAME DIR IN HOME DIRECTORY
    QDir dir(GAMEDIR);
    //If the game dir doesn't exist create it
    if (!dir.exists())
    {
        if (flag & DEBUG) logWarn("Game dir doesn't exist!");
        dir.mkdir(GAMEDIR);
        dir.cd(GAMEDIR);
        //create subdir for user levels and progressions.
        dir.mkdir("userLevels");
        dir.mkdir("userProgression");
        dir.mkdir("debug");
    }
    else if (flag & DEBUG) logWarn("Game dir exist!");

    readConfiguration();

#ifdef Q_OS_WIN32
    _gameEngine = new OGGameEngine(_config.screen_width
                                   , _config.screen_height
                                   , _config.fullscreen);
#else
    _gameEngine = new OGGameEngine(_config.screen_width
                                   , _config.screen_height
                                   , false);
#endif //   Q_OS_WIN32

    if (_gameEngine == 0) { return false; }

    _gameEngine->setFrameRate(FRAMERATE);

    _width = _config.screen_width;
    _height = _config.screen_height;

    return true;
}

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
}

void GameEnd()
{
    clearUI();

    delete _gameEngine;

    if (_world)
    {
        logDebug("Clear world");

        delete _world;
    }
}

void GameCycle()
{
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

    calculateFPS();

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

    if (!_listStates.isEmpty())
    {
        OGEvent* e = _listStates.takeFirst();

        switch (e->type())
        {
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
            if (button->onclick() == "quit") { closeGame(); }
            else if (button->onclick() == "credits") { }
            else if (button->onclick() == "showselectprofile") { }
            else if (button->onclick() == "island1")
            {
                _listStates << new OGIslandEvent("island1");
            }
            else if (!button->onclick().isEmpty())
            {
                QString name = button->getLevelName();

                if (!name.isEmpty())
                {
                    _listStates << new OGLevelEvent(name);
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

void gooMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type)
    {
        case QtDebugMsg:
            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            abort();
    }
}

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

QPointF logicalToWindow(const QRectF &rect, qreal zoom)
{
    qreal w = rect.width() / zoom;
    qreal h = rect.height() / zoom;
    qreal x = rect.x() - w * 0.5f;
    qreal y = (rect.y() + h * 0.5f) * (-1.0f);

    return QPointF(x, y);
}

void closeGame() { _gameEngine->getWindow()->close(); }

void readConfiguration()
{
    // Read game configuration
    QString filename("./resources/config.xml");
    OGGameConfig gameConfig(filename);

    if (gameConfig.Open())
    {
        if (gameConfig.Read()) { _config = gameConfig.Parser(); }
        else {logWarn("File " + filename + " is corrupted"); }
    }
    else
    {
        logWarn("File " + filename + " not found");
        gameConfig.Create(_config);
    }
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

void calculateFPS()
{
    using namespace FPSCounter;

    sumFPSTime += _lastTime;
    _cur_fps++;

    if (sumFPSTime >= 1000)
    {
        if (_cur_fps > FRAMERATE) { _fps = FRAMERATE; }
        else { _fps = _cur_fps; }

        _cur_fps = 0;
        sumFPSTime = 0;
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
