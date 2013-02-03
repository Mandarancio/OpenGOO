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
#include <QTransform>
#include <QtAlgorithms>
#include <cstdio>
#include <QtCore/qmath.h>

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
        else if (!arg.compare("-fps",Qt::CaseInsensitive))
        {
            flag |= FPS;
        }
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

    _gameEngine->setFrameRate(60);     

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
    _numberCameras = _world->GetNumberCameras();

    if (!createPhysicsWorld()) { return; }

    _isLevelInitialize = true;        

    buttonMenu();

    if (_numberCameras > 1) { _isMoveCamera = true; }
    else { _isMoveCamera = false; }

    if (_world->leveldata()->visualdebug)
    {
        _time.start();
    }
}

void GameEnd()
{
    _isLevelInitialize = false;

    delete _gameEngine;

    clearPhysicsWorld();

    if (_world)
    {
        logDebug("Clear world");

        delete _world;
    }
}

void GameCycle()
{
    if(!_isLevelInitialize)
    {
        closeGame();

        return;
    }

    _physicsEngine->Simulate();

    if (_isMoveCamera)
    {
        _isZoomLock = true;
        moveCamera();
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

    moveBall();

    _gameEngine->getWindow()->render();
}

void GamePaint(QPainter* painter)
{    
    // Test

    if (!_isLevelInitialize)
    {
        return;
    }

    painter->setWindow(_camera.window().toRect());
    painter->setViewport(0, 0, _width, _height);

    painter->setRenderHint(QPainter::HighQualityAntialiasing);

    setBackgroundColor(_world->scenedata()->backgroundcolor);
    drawOpenGLScene();

    // Draw scene
    for (int i=0; i< _sprites->size(); i++)
    {
        if (_sprites->at(i)->visible)
        {
            painter->setOpacity(_sprites->at(i)->alpha);
            qreal x = _sprites->at(i)->position.x();
            qreal y = _sprites->at(i)->position.y();
            qreal w = _sprites->at(i)->size.width();
            qreal h = _sprites->at(i)->size.height();
            painter->drawPixmap(QRectF(x, y, w, h), _sprites->at(i)->image
                                , _sprites->at(i)->image.rect());
        }
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

void GameActivate() { _isPause = false; }

void GameDeactivate() { _isPause = true; }

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

    for(int i=0; i < _buttons->size(); i++)
    {
        QRectF button(QPointF(_buttons->at(i)->position().x()
                              , _buttons->at(i)->position().y())
                      , _buttons->at(i)->size());

        button.moveCenter(_buttons->at(i)->position());

        if(button.contains(mPos))
        {
            _E404 = false;

            if (_buttons->at(i)->onclick() == "quit") { closeGame(); }
            else if (_buttons->at(i)->onclick() == "credits") { }
            else if (_buttons->at(i)->onclick() == "showselectprofile") { }
            else { _E404 = true; }
        }
    }

    for (int i=0; i < _balls.size(); i++)
    {
        if (_balls.at(i)->active)
        {
            qreal x, y, radius;
            x = _balls.at(i)->ball->body->GetPosition().x*K;
            y = _balls.at(i)->ball->body->GetPosition().y*K;
            radius = _balls.at(i)->ball->shape->GetRadius()*K;

            QLineF ball(x, y, mPos.x(), mPos.y());

            if(ball.length() <= radius)
            {
                _balls.at(i)->selected = true;
                _selectedBall = i;
                _balls.at(i)->ball->body->SetAwake(false);
                break;
            }
        }
    }
}

void MouseButtonUp(QMouseEvent* event)
{
    Q_UNUSED(event)

    if (_selectedBall != -1)
    {
        _balls.at(_selectedBall)->selected = false;
        _balls.at(_selectedBall)->ball->body->SetAwake(true);
        if (_tmpStrands.size()>1)
        {
            for (int i=0; i < _tmpStrands.size(); i++)
            {
                _strands << createStrand(_selectedBall, _tmpStrands.at(i).gb1);
            }
        }

        _selectedBall = -1;
        _tmpStrands.clear();
    }
}

void MouseMove(QMouseEvent* event)
{
    const qreal OFFSET = 50.0;

    qreal x, y;

    QPoint mPos(event->pos());

    x = mPos.x();
    y = mPos.y();
    Scroll scroll = {false, false, false, false};
    _scroll = scroll;
    mPos = windowToLogical(mPos);

    if (_buttons)
    {
        for(int i=0; i < _buttons->size(); i++)
        {
            QRectF button(QPointF(_buttons->at(i)->position().x()
                                  , _buttons->at(i)->position().y())
                          , _buttons->at(i)->size());

            button.moveCenter(_buttons->at(i)->position());

            if(button.contains(mPos))
            {
                _buttons->at(i)->up()->visible = false;
                _buttons->at(i)->over()->visible = true;
            }
            else
            {
                _buttons->at(i)->up()->visible = true;
                _buttons->at(i)->over()->visible = false;
            }
        }
    }

    if (_selectedBall != -1)
    {
        qreal x, y;

        x = mPos.x()*0.1;
        y = mPos.y()*0.1;
        b2Body* body = _balls.at(_selectedBall)->ball->body;

        body->SetTransform(b2Vec2(x, y), body->GetAngle());
        _balls.at(_selectedBall)->ball->body->SetAwake(false);

        _tmpStrands.clear();

        for (int i=0; i < _balls.size(); i++)
        {
            if (!_balls.at(i)->active)
            {
                qreal x2, y1, y2, length;
                x2 = _balls.at(i)->ball->body->GetPosition().x*K;
                y2 = _balls.at(i)->ball->body->GetPosition().y*K;
                length = QLineF(mPos.x(), mPos.y(), x2, y2).length();

                if (length >= 100 && length <= 200)
                {
                    y1  = mPos.y()*(-1.0);
                    y2 *= -1.0;
                    _tmpStrands << OGStrand(i, QLineF(mPos.x(), y1, x2, y2));
                }
            }
        }
    }

    if (x <= OFFSET) { _scroll.left = true; }
    else if (x >= _width - OFFSET) { _scroll.right = true; }

    if (y <= OFFSET) { _scroll.up = true; }
    else if (y >= _height - OFFSET) { _scroll.down = true; }
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

    const qreal SHIFT = 10;

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

void moveCamera()
{
    if (_numberCameras < 2)
    {
        _isMoveCamera = false;

        if (_world->leveldata()->visualdebug) { _isZoomLock = false; }

        return;
    }

    if (_frames <= 0)
    {
        const OGCamera& endCam = _world->GetCamera(_nextCamera);
        qreal x = endCam.center().x() - _camera.center().x();
        qreal y = endCam.center().y() - _camera.center().y();
        qreal zoom = endCam.zoom() - _camera.zoom();

        qreal fps =  1000.0/_gameEngine->getFrameDelay();
        _pause = endCam.pause() * fps;
        _frames = endCam.traveltime()*fps;

        if (_frames > 0)
        {
            _dx = x/_frames;
            _dy = y/_frames;
            _dzoom = zoom/_frames;
        }
    }

    if (_pause > 0)
    {
        _pause--;
        return;
    }

    if (_frames > 0)
    {
        _camera.SetPosition(_camera.center().x() + _dx,
                            _camera.center().y() + _dy);
        _camera.Scale(_camera.zoom() + _dzoom);
        _frames--;
    }

    if (_frames <= 0)
    {
        _world->SetNextCamera();
        _numberCameras--;
        _nextCamera++;
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
    if (_physicsEngine)
    {
        logError("Invalid physics engine.");
        return false;
    }

    QPointF gravity;
    WOGMaterial* material;
    QString id; // material name

    if (!_world->scenedata()->linearforcefield.isEmpty())
    {
        if (_world->scenedata()->linearforcefield.last()->type == "gravity")
        {
            gravity = _world->scenedata()->linearforcefield.last()->force;
        }
    }

    if (!initializePhysicsEngine(gravity, true))
    {
        logError("Unable init physics engine.");
        return false;
    }

    _physicsEngine = OGPhysicsEngine::GetInstance();

    for (int i=0; i < _world->scenedata()->circle.size(); i++)
    {
        if (!_world->scenedata()->circle.at(i)->dynamic)
        {
            WOGCircle* circle = _world->scenedata()->circle.at(i);
            id = circle->material;
            material = _world->materialdata()->GetMaterial(id);

            if(material)
            {
                OGStaticBody* body = new OGStaticBody;
                body->body = createCircle(circle->position, circle->radius
                                          , material, false, 0, body);

                body->tag = circle->tag;

                _staticCircles << body;
            }
            else { logError(QString("Wring material id: %1").arg(id)); }
        }
    }

    for (int i=0; i < _world->scenedata()->line.size(); i++)
    {
        if (!_world->scenedata()->line.at(i)->dynamic)
        {
            WOGLine* line = _world->scenedata()->line.at(i);
            id = line->material;
            material = _world->materialdata()->GetMaterial(id);                        

            if(material)
            {
                OGStaticBody* body = new OGStaticBody;
                body->body = createLine(line->anchor, line->normal, material
                                        , _world, false, body);

                body->tag = line->tag;

                _staticLines << body;
            }
            else { logError(QString("Wring material id: %1").arg(id)); }
        }
    }

    for (int i=0; i < _world->scenedata()->rectangle.size(); i++)
    {
        if (!_world->scenedata()->rectangle.at(i)->dynamic)
        {
            WOGRectangle* rect = _world->scenedata()->rectangle.at(i);
            id = rect->material;
            material = _world->materialdata()->GetMaterial(id);

            if(material)
            {
                OGStaticBody* body = new OGStaticBody;
                body->body = createRectangle(rect->position, rect->size
                                             , rect->rotation , material, false
                                             , body);

                body->tag = rect->tag;

                _staticRectangles << body;
            }
            else { logError(QString("Wring material id: %1").arg(id)); }
        }
    }

    for (int i=0; i < _world->leveldata()->ball.size(); i++)
    {
        WOGBallInstance* data = _world->leveldata()->ball.at(i);
        WOGBall* configuration = readBallConfiguration(data->type);

        if (configuration)
        {
            _balls << createBall(data, configuration);

            delete configuration;
        }
    }

    for (int i=0; i < _world->leveldata()->strand.size(); i++)
    {
        _strands << createStrand(_world->leveldata()->strand.at(i));
    }

    _physicsEngine->SetSimulation(6, 2, 60.0);

    return true;
}

void clearPhysicsWorld()
{
    if (!_physicsEngine) { return; }

    OGPhysicsEngine::DestroyInstance();

    _physicsEngine = 0;

    while (!_balls.isEmpty()) { delete _balls.takeFirst(); }

    while (!_staticCircles.isEmpty()) { delete _staticCircles.takeFirst(); }

    while (!_staticLines.isEmpty()) { delete _staticLines.takeFirst(); }

    while (!_staticRectangles.isEmpty())
    {
        delete _staticRectangles.takeFirst();
    }

    while (!_strands.isEmpty()) { delete _strands.takeFirst(); }
}

void readConfiguration()
{
    // Read game configuration
    QString filename("./resources/config.xml");
    OGGameConfig gameConfig(filename);

    if (gameConfig.Open())
    {
        if (gameConfig.Read())
        {
            _config = gameConfig.Parser();
        }
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

void drawOpenGLScene()
{

}

WOGBall* readBallConfiguration(const QString & dirname)
{
    QString path = "./res/balls/" + dirname + "/balls.xml";
    OGBallConfig config(path);

    if (config.Open())
    {
        if (config.Read())
        {
            return config.Parser();
        }
        else {logWarn("File " + path + " is corrupted"); }
    }
    else { logWarn("File " + path +" not found"); }

    return 0;
}

OGBall* createBall(WOGBallInstance* data, WOGBall* configuration)
{
    float32 radius;

    OGBall* obj = new OGBall;
    obj->id = data->id;
    WOGMaterial ballmaterial = {QString(), 15.0, 0.1, 102, 30};
    QStringList list = configuration->attribute.core.shape.split(",");

    if (list.at(0) == "circle")
    {
        radius = list.at(1).toFloat()*0.5;

        if (list.size() == 3)
        {
            int n = list[2].toFloat()*100;

            if (n > 100) { n = 100; }

            if (n >= 1) { radius += radius*(qrand()%n)*0.01; }
        }

        obj->ball = createCircle(data->position, radius, &ballmaterial
                               , true, configuration->attribute.core.mass);

        obj->active = true;
    }
    else if (list.at(0) == "rectangle")
    {

    }

    return obj;
}

OGStrand* createStrand(WOGStrand* strand)
{
    OGStrand* obj = new OGStrand;
    bool isFound1 = false;
    bool isFound2 = false;

    for(int i=0; i < _balls.size(); i++)
    {
        if (_balls.at(i)->ball)
        {
            if (strand->gb1 == _balls.at(i)->id)
            {
                obj->gb1 = i;
                isFound1 = true;
            }
            else if (strand->gb2 == _balls.at(i)->id)
            {
                obj->gb2 = i;
                isFound2 = true;
            }

            if (isFound1 && isFound2)
            {
                obj->strand = createJoint(_balls.at(obj->gb1)->ball
                                          , _balls.at(obj->gb2)->ball);

                _balls.at(obj->gb1)->active = false;
                _balls.at(obj->gb2)->active = false;

                break;
            }
        }
    }

    return obj;
}

OGStrand* createStrand(int b1, int b2)
{
    OGStrand* obj = new OGStrand;
    obj->gb1 = b1;
    obj->gb2 = b2;
    obj->strand = createJoint(_balls.at(obj->gb1)->ball
                              , _balls.at(obj->gb2)->ball);

    _balls.at(obj->gb1)->active = false;
    _balls.at(obj->gb2)->active = false;

    return obj;
}

void moveBall()
{
    qreal x, y;

    const QPointF & pos = getNearestPosition();

    for (int i=0; i < _balls.size(); i++)
    {
        if (_balls.at(i)->active && !_balls.at(i)->selected
                && testWalkable(_balls.at(i)->ball))
        {

            b2Vec2 vel = _balls.at(i)->ball->body->GetLinearVelocity();
            x = _balls.at(i)->ball->body->GetPosition().x;
            y = _balls.at(i)->ball->body->GetPosition().y;

            if ((pos.x() - x) >= 0)
            {
                vel.x = 5;
            }
            else
            {
                vel.x = -5;
            }

            _balls.at(i)->ball->body->SetLinearVelocity(vel);
        }
    }
}

QPointF getNearestPosition()
{
    qreal x, y, x1, y1, x2, y2, length, tmpLength;
    bool isInitialize;

    isInitialize = true;

    for (int i=0; i < _balls.size(); i++)
    {
        if (!_balls.at(i)->active)
        {
            if (isInitialize)
            {
                x1 = _world->leveldata()->levelexit->pos.x()*0.1;
                y1 = _world->leveldata()->levelexit->pos.y()*0.1;
                x2 = _balls.at(i)->ball->body->GetPosition().x;
                y2 = _balls.at(i)->ball->body->GetPosition().y;
                x = x2;
                y = y2;
                length = QLineF(x1, y1, x2, y2).length();
                isInitialize = false;
            }
            else
            {
                x2 = _balls.at(i)->ball->body->GetPosition().x;
                y2 = _balls.at(i)->ball->body->GetPosition().y;
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

bool testWalkable(OGPhysicsBody* body)
{
    OGStaticBody* data;
    b2ContactEdge* edge = body->body->GetContactList();

    while (edge)
    {
        if (data = static_cast<OGStaticBody*>(edge->other->GetUserData()))
        {
            if (data->tag == "walkable") { return true; }
        }

        edge = edge->next;
    }

    return false;
}
