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

bool GameInitialize(int argc, char **argv)
{    
    #ifndef Q_OS_WIN32
    BackTracer(SIGSEGV);
    BackTracer(SIGFPE);
    #endif
    #ifdef Q_OS_WIN32
    AddVectoredExceptionHandler(0, UnhandledException2);
    #endif

    ConsoleAppender * con_apd;
    con_apd  = new ConsoleAppender(LoggerEngine::LevelInfo,stdout,"%d - <%l> - %m%n");
    LoggerEngine::addAppender(con_apd);
    con_apd  = new ConsoleAppender(LoggerEngine::LevelDebug     |
                                   LoggerEngine::LevelWarn      |
                                   LoggerEngine::LevelCritical  |
                                   LoggerEngine::LevelError     |
                                   LoggerEngine::LevelException |
                                   LoggerEngine::LevelFatal,stdout,"%d - <%l> - %m [%f:%i]%n");
    LoggerEngine::addAppender(con_apd);
    qInstallMessageHandler(gooMessageHandler);

    //Check for the run parameters
    for (int i=1; i<argc; i++)
    {
        QString arg(argv[i]);
        //Check for debug Option
        if (!arg.compare("-debug", Qt::CaseInsensitive))
        {
            flag |= DEBUG;
            logWarn("DEBUG MODE ON");
        }
        else if (!arg.compare("-text", Qt::CaseInsensitive))
        {
            flag |= ONLYTEXT | DEBUG;
        }
        else if (!arg.compare("-fps",Qt::CaseInsensitive)) { flag |= FPS; }
        else { _levelname = arg; }
    }

    //CHECK FOR GAME DIR IN HOME DIRECTORY
    QDir dir(GAMEDIR);
    //If the game dir doesn't exist create it
    if (!dir.exists()) {
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
                                   , _config.fullscreen
                                   );
#else
    _gameEngine = new OGGameEngine(_config.screen_width
                                   , _config.screen_height
                                   , false
                                   );
#endif //   Q_OS_WIN32

    if (_gameEngine == 0) { return false; }

    _gameEngine->setFrameRate(FRAMERATE);

    return true;
}

void GameStart()
{
    //initialize randseed
    qsrand(QTime::currentTime().toString("hhmmsszzz").toUInt());

    _width = _gameEngine->getWidth();
    _height = _gameEngine->getHeight();
    _E404 = false;
    _isScrollLock = false;
    _isZoomLock = false;
    _isLevelInitialize = false;
    _timeScrollStep = _width/1000;

    // MapWorldView is the main menu

    if(_levelname.isEmpty() || (!OGWorld::isExist(_levelname)) )
    {
     _levelname = "MapWorldView";
    }

    _world = new OGWorld(_levelname);

    _world->language(_config.language);

    if (!_world->Initialize()) { return; }

    if (!_world->Load()) { return; }

    _world->CreateScene();
    _sprites = _world->sprites();
    _buttons = _world->buttons();
    _camera = _world->currentcamera();
    camera::numberCameras = _world->GetNumberCameras();

    _physicsEngine = OGPhysicsEngine::GetInstance();
    _physicsEngine->SetSimulation(6, 2, STEPS);

    _isLevelInitialize = true;

    buttonMenu();

    if (camera::numberCameras > 1) { _isMoveCamera = true; }
    else { _isMoveCamera = false; }

    if (_world->leveldata()->visualdebug) { _time.start(); }

    _timeStep = STEPS/1000.0;
}

void GameEnd()
{
    _isLevelInitialize = false;

    delete _gameEngine;
    delete _gameTime;

    _physicsEngine = 0;

    if (_world)
    {
        logDebug("Clear world");

        delete _world;
    }
}

void GameCycle()
{
    if (!_isLevelInitialize)
    {
        closeGame();

        return;
    }

    if (!_gameTime)
    {
        _gameTime = new QTime;

        _gameTime->start();
    }

    _lastTime = _gameTime->restart();
    _lastTime = 16;

    calculateFPS();

    int n = qRound(_lastTime*_timeStep);

    for (int i=0; i < n; i++)
    {
        _physicsEngine->Simulate();
    }

    if (_isMoveCamera)
    {
        _isZoomLock = true;
        updateCamera(_lastTime);
    }
    else
    {
        scroll();

        if (_scrolltime > 0) { _scrolltime--; }
        else if (_scrolltime == 0)
        {
            _scroll.up = false;
            _scroll.down = false;
            _scrolltime--;
        }
    }

    // Receive position of nearest vertex from the exit (approximately once per second)

    if (_nearestCounter == 0)
    {
        _nearestPosition = getNearestPosition();
        _nearestCounter++;
    }
    else if (_nearestCounter >= _fps)
    {
        _nearestCounter = 0;
    }
    else { _nearestCounter++; }

    moveBall();

    _gameEngine->getWindow()->render();
}

void GamePaint(QPainter* painter)
{    
    if (!_isLevelInitialize) { return; }

    painter->setWindow(_camera.window().toRect());
    painter->setViewport(0, 0, _width, _height);
    painter->setRenderHint(QPainter::HighQualityAntialiasing);

    setBackgroundColor(_world->scenedata()->backgroundcolor);
    drawOpenGLScene();

    // Paint a scene
    Q_FOREACH (OGSprite* sprite, *_sprites)
    {
        if (sprite->visible)
        {
            painter->setOpacity(sprite->alpha);
            qreal x = sprite->position.x();
            qreal y = sprite->position.y();
            qreal w = sprite->size.width();
            qreal h = sprite->size.height();
            painter->drawPixmap(QRectF(x, y, w, h), sprite->image
                                , sprite->image.rect());
        }
    }

    Q_FOREACH (OGBall* ball, *_world->balls())
    {
        ball->Paint(painter, _world->leveldata()->visualdebug);
    }

    Q_FOREACH (OGStrand* strand, *_world->strands())
    {
        strand->Paint(painter, _world->leveldata()->visualdebug);
    }

    if (_isPause)
    {
        painter->setOpacity(0.25);
        painter->fillRect(_camera.window(), Qt::black);
        painter->setOpacity(1);
        painter->setPen(Qt::white);
        painter->setFont(QFont("Times", 48, QFont::Bold));
        painter->drawText(_camera.window(), Qt::AlignCenter, "Pause");
    }

    if (_world->leveldata()->visualdebug)
    {
        visualDebug(painter, _world, _camera.zoom());
    }
}

void GameActivate()
{
    _isPause = false;

}

void GameDeactivate()
{
    _isPause = true;
}

void KeyDown(QKeyEvent* event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
        _E404 = false;
        break;
    }
}

void KeyUp(QKeyEvent* event) { Q_UNUSED(event) }

void MouseButtonDown(QMouseEvent* event)
{
    if (!_buttons) { return; }

    QPoint mPos(event->pos());
    QRectF menu(_buttonMenu.position(), _buttonMenu.size());

    if (menu.contains(mPos)) { buttonMenuAction(); }

    mPos = windowToLogical(mPos);

    Q_FOREACH (OGButton* button, *_buttons)
    {
        QRectF rect(QPointF(button->position().x(), button->position().y())
                    , button->size());

        rect.moveCenter(button->position());

        if(rect.contains(mPos))
        {
            _E404 = false;

            if (button->onclick() == "quit") { closeGame(); }
            else if (button->onclick() == "credits") { }
            else if (button->onclick() == "showselectprofile") { }
            else { _E404 = true; }
        }
    }

    if (_selectedBall)
    {
        _selectedBall->Event("pickup");
    }
}

void MouseButtonUp(QMouseEvent* event)
{
    Q_UNUSED(event)

    if (_selectedBall)
    {
        if (_selectedBall->dragging)
        {
            Q_FOREACH (OGBall* ball, _balls)
            {
                _selectedBall->Attache(ball);
            }

            _selectedBall->dragging = false;            
            _selectedBall->standing = false;

            _selectedBall->body->SetAwake(true);
            _selectedBall = 0;

             _balls.clear();
        }
    }
}

void MouseMove(QMouseEvent* event)
{
    const qreal OFFSET = 50.0;

    qreal sx, sy;

    QPoint mPos(event->pos());

    sx = mPos.x();
    sy = mPos.y();
    Scroll scroll = {false, false, false, false};
    _scroll = scroll;
    mPos = windowToLogical(mPos);

    if (_buttons)
    {
        Q_FOREACH (OGButton* _button, *_buttons)
        {
            QRectF rect(QPointF(_button->position().x()
                                , _button->position().y())
                          , _button->size());

            rect.moveCenter(_button->position());

            if(rect.contains(mPos))
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
    }

    if (_selectedBall)
    {
        float32 x, y;

        x = mPos.x()*physics::K;
        y = mPos.y()*physics::K;

        if (_selectedBall->dragging)
        {
            if (_selectedBall->attached)
            {
                _selectedBall->Detache();
            }
            else
            {
                if (_selectedBall->GetMaxStrands() != 0)
                {
                    _selectedBall->SetBodyPosition(x, y);

                    findConnectBalls();
                }
            }
        }
        else
        {
            if (!_selectedBall->Select(mPos))
            {
                _selectedBall->standing = false;
                _selectedBall->body->SetAwake(true);
                _selectedBall = 0;
            }
        }
    }
    else
    {
        Q_FOREACH (OGBall* ball, *_world->balls())
        {
            if (ball->Select(mPos))
            {
                _selectedBall = ball;
                _selectedBall->Event("marker");
                break;
            }
        }
    }

    if (sx <= OFFSET) { _scroll.left = true; }
    else if (sx >= _width - OFFSET) { _scroll.right = true; }

    if (sy <= OFFSET) { _scroll.up = true; }
    else if (sy >= _height - OFFSET) { _scroll.down = true; }
}

void MouseWheel(QWheelEvent* event)
{
    if (_isMoveCamera) { return; }

    QPoint numDegrees = event->angleDelta() / 8;

    if (_world->leveldata()->visualdebug)
    {        
        if (numDegrees.y() > 0) { zoom(1); }
        else { zoom(-1); }
    }
    else
    {
        if (numDegrees.y() > 0)
        {
            _scroll.up = true;
            _scrolltime = 30;
        }
        else
        {
            _scroll.down = true;
            _scrolltime = 30;
        }
    }
}

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
    if (_isScrollLock) { return; }

    qreal SHIFT = _timeScrollStep*_lastTime;

    qreal pos;

    if (_scroll.up)
    {        
        pos = qMin(_camera.center().y() + SHIFT,
                 _world->scenedata()->maxy - _camera.height()*0.5);

        _camera.SetY(pos);
    }
    else if (_scroll.down)
    {
        pos = qMax(_camera.center().y() - SHIFT,
                 _world->scenedata()->miny + _camera.height()*0.5);

        _camera.SetY(pos);
    }

    if (_scroll.left)
    {
        pos = qMax(_camera.center().x() - SHIFT,
                 _world->scenedata()->minx + _camera.width()*0.5);

        _camera.SetX(pos);
    }
    else if (_scroll.right)
    {
        pos = qMin(_camera.center().x() + SHIFT,
                 _world->scenedata()->maxx - _camera.width()*0.5);

        _camera.SetX(pos);
    }
}

void zoom(int direct)
{
    if (_isZoomLock) { return; }

    qreal zoom = _camera.zoom();

    zoom += (0.1*direct);

    if (_world->scenesize().width()*zoom >= _width
            && _world->scenesize().height()*zoom >= _height
       )
    {
        _camera.Scale(zoom);
    }
}

QPointF logicalToWindow(const QRectF & rect, qreal zoom)
{
    qreal w = rect.width()/zoom;
    qreal h = rect.height()/zoom;
    qreal x = rect.x() - w*0.5;
    qreal y = (rect.y() + h*0.5)*(-1.0);

    return QPointF(x, y);
}

QPoint windowToLogical(const QPoint & position)
{
    qreal x, y, a, b;

    a = (position.x() - _width*0.5)/_camera.zoom();
    b = (position.y() - _height*0.5)/_camera.zoom();
    x = _camera.center().x() + a;
    y = _camera.center().y() - b;

    return QPoint(x, y);
}

void updateCamera(int time)
{
    if (camera::numberCameras < 2)
    {
        _isMoveCamera = false;

        if (_world->leveldata()->visualdebug) { _isZoomLock = false; }

        return;        
    }

    if (camera::isInitialization)
    {
        camera::sumTime = 0;
        camera::isPause = false;

        OGCamera* endCam = _world->GetCamera(camera::nextCamera);
        qreal dx = endCam->center().x() - _camera.center().x();
        qreal dy = endCam->center().y() - _camera.center().y();
        qreal dzoom = endCam->zoom() - _camera.zoom();
        camera::pause = endCam->pause()* 1000;

        if (camera::pause) { camera::isPause = true; }

        camera::traveltime = endCam->traveltime()*1000;

        camera::xSpeed = dx/camera::traveltime;
        camera::ySpeed = dy/camera::traveltime;
        camera::zoomSpeed = dzoom/camera::traveltime;
        camera::isInitialization = false;
    }

    if (camera::isPause)
    {
        int lastTime = time;
        camera::sumTime += lastTime;

        if ( camera::sumTime <= camera::pause) { return; }
        else
        {
            camera::isPause = false;
            camera::sumTime = 0;
        }
    }
    else
    {
        int lastTime = time;
        camera::sumTime += lastTime;

        if (camera::sumTime <= camera::traveltime)
        {
            _camera.SetPosition(_camera.center().x() + camera::xSpeed*lastTime,
                                _camera.center().y() + camera::ySpeed*lastTime);
            _camera.Scale(_camera.zoom() + camera::zoomSpeed*lastTime);
        }
        else
        {
            _world->SetNextCamera();
            camera::numberCameras--;
            camera::nextCamera++;
            camera::isInitialization = true;
        }
    }
}

void closeGame() { _gameEngine->getWindow()->close(); }

void buttonMenuAction() { closeGame(); }

void buttonMenu()
{
    qreal offset, btnW, btnH, menuX, menuY;

    _buttonMenu.onclick("menu");
    _buttonMenu.size(QSize(50, 20));
    offset = 10.0;
    btnW = _buttonMenu.size().width();
    btnH = _buttonMenu.size().height();
    menuX = _width - (btnW + offset);
    menuY = _height - (btnH + offset);
    _buttonMenu.position(QPointF(menuX, menuY));
}

bool createPhysicsWorld()
{


    return true;
}

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
        logWarn("File " + filename +" not found");
        gameConfig.Create(_config);
    }
}

void setBackgroundColor(const QColor & color)
{    
    glClearColor(color.redF(), color.greenF(), color.blueF(), 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void drawOpenGLScene() {}

void moveBall()
{
    Q_FOREACH (OGBall* ball, *_world->balls())
    {
        ball->Move(_nearestPosition);
    }
}

QPointF getNearestPosition()
{
    qreal x, y, x1, y1, x2, y2, length, tmpLength;
    bool isInitialize;

    isInitialize = true;

    Q_FOREACH (OGBall* ball, *_world->balls())
    {
        if (ball->attached)
        {
            if (isInitialize)
            {
                x1 = _world->leveldata()->levelexit->pos.x()*0.1;
                y1 = _world->leveldata()->levelexit->pos.y()*0.1;
                x2 = ball->GetX();
                y2 = ball->GetY();
                x = x2;
                y = y2;
                length = QLineF(x1, y1, x2, y2).length();
                isInitialize = false;
            }
            else
            {
                x2 = ball->GetX();
                y2 = ball->GetY();
                tmpLength = QLineF(x1, y1, x2, y2).length();

                if (tmpLength < length)
                {
                    length = tmpLength;
                    x = x2;
                    y = y2;
                }
            }
        }
    }

    return QPointF(x, y);
}

void calculateFPS()
{
    _cur_fps++;

    if (_time.elapsed() >= 1000)
    {
        if (_cur_fps > FRAMERATE) { _fps = FRAMERATE; }
        else { _fps = _cur_fps; }

        _cur_fps = 0;

        _time.start();
    }
}

void findConnectBalls()
{
    qreal length;
    int maxStrands = _selectedBall->GetMaxStrands();
    float minlen = _selectedBall->m_configuration->stand->minlen;
    float maxlen1 = _selectedBall->m_configuration->stand->maxlen1;

    _balls.clear();

    Q_FOREACH (OGBall* ball, *_world->balls())
    {
        if (ball->attached)
        {
            length = b2Distance(ball->GetBodyPosition()
                                , _selectedBall->GetBodyPosition())*10;

            if (length >= minlen && length <= maxlen1)
            {
                _balls << ball;

                if (_balls.size() == maxStrands)
                {
                    break;
                }
            }
        }
    }

    if (maxStrands >= 2 && _balls.size() < 2) { _balls.clear(); }
}
