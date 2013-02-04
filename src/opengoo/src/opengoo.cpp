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

    if (_world->leveldata()->visualdebug) { _time.start(); }
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
    if (!_isLevelInitialize)
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

    int i = 0;

    Q_FOREACH (OGBall* ball, _balls)
    {
        if (ball->active)
        {
            qreal x, y, radius;
            x = ball->GetX()*K;
            y = ball->GetY()*K;
            radius = ball->body->shape->GetRadius()*K;
            qreal length = QLineF(x, y, mPos.x(), mPos.y()).length();

            if (length <= radius)
            {
                ball->selected = true;
                _selectedBall = i;
                ball->body->body->SetAwake(false);
                break;
            }
        }

        i++;
    }
}

void MouseButtonUp(QMouseEvent* event)
{
    Q_UNUSED(event)

    if (_selectedBall != -1)
    {
        _balls.at(_selectedBall)->selected = false;
        _balls.at(_selectedBall)->body->body->SetAwake(true);
        if (_tmpStrands.size()>1)
        {
            Q_FOREACH (OGStrand strand, _tmpStrands)
            {
                _strands << createStrand(_selectedBall, strand.gb1);
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

    if (_selectedBall != -1)
    {
        qreal x, y;

        x = mPos.x()*0.1;
        y = mPos.y()*0.1;
        b2Body* body = _balls.at(_selectedBall)->body->body;

        body->SetTransform(b2Vec2(x, y), body->GetAngle());
        _balls.at(_selectedBall)->body->body->SetAwake(false);

        _tmpStrands.clear();

        int i = 0;

        Q_FOREACH (OGBall* ball, _balls)
        {
            if (!ball->active)
            {
                qreal x2, y1, y2, length;
                x2 = ball->GetX()*K;
                y2 = ball->GetY()*K;
                length = QLineF(mPos.x(), mPos.y(), x2, y2).length();

                if (length >= 100 && length <= 200)
                {
                    y1  = mPos.y()*(-1.0);
                    y2 *= -1.0;
                    _tmpStrands << OGStrand(i, QLineF(mPos.x(), y1, x2, y2));
                }
            }

            i++;
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

    Q_FOREACH (WOGCircle* circle, _world->scenedata()->circle)
    {
        if (!circle->dynamic)
        {
            id = circle->material;
            material = _world->materialdata()->GetMaterial(id);

            if (material)
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

    Q_FOREACH (WOGLine* line, _world->scenedata()->line)
    {
        if (!line->dynamic)
        {
            id = line->material;
            material = _world->materialdata()->GetMaterial(id);                        

            if (material)
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

    Q_FOREACH (WOGRectangle* rect, _world->scenedata()->rectangle)
    {
        if (!rect->dynamic)
        {
            id = rect->material;
            material = _world->materialdata()->GetMaterial(id);

            if (material)
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

    Q_FOREACH (WOGBallInstance* data, _world->leveldata()->ball)
    {
        WOGBall* configuration = readBallConfiguration(data->type);

        if (configuration)
        {
            _balls << createBall(data, configuration);

            delete configuration;
        }
    }

    Q_FOREACH (WOGStrand* strand, _world->leveldata()->strand)
    {
        _strands << createStrand(strand);
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

void drawOpenGLScene()
{

}

WOGBall* readBallConfiguration(const QString & dirname)
{
    QString path = "./res/balls/" + dirname + "/balls.xml";
    OGBallConfig config(path);

    if (config.Open())
    {
        if (config.Read()) { return config.Parser(); }
        else { logWarn("File " + path + " is corrupted"); }
    }
    else { logWarn("File " + path +" not found"); }

    return 0;
}

OGBall* createBall(WOGBallInstance* data, WOGBall* configuration)
{
    float32 radius;

    OGBall* obj = new OGBall;
    obj->id = data->id;    
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

        obj->body = createCircle(data->position, radius, &_ballmaterial
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
    int i = 0;

    Q_FOREACH (OGBall* ball, _balls)
    {
        if (ball->IsValid())
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
                obj->strand = createJoint(_balls.at(obj->gb1)->body
                                          , _balls.at(obj->gb2)->body);

                _balls.at(obj->gb1)->active = false;
                _balls.at(obj->gb2)->active = false;

                break;
            }
        }

        i++;
    }

    return obj;
}

OGStrand* createStrand(int b1, int b2)
{
    OGStrand* obj = new OGStrand;
    obj->gb1 = b1;
    obj->gb2 = b2;
    obj->strand = createJoint(_balls.at(b1)->body, _balls.at(b2)->body);
    _balls.at(b1)->active = false;
    _balls.at(b2)->active = false;

    return obj;
}

void moveBall()
{
    const QPointF & pos = getNearestPosition();

    Q_FOREACH (OGBall* ball, _balls)
    {
        if (ball->active && !ball->selected && testWalkable(ball->body))
        {         
            b2Vec2 vel = ball->body->body->GetLinearVelocity();
            qreal dx = pos.x() - ball->GetX();

            if (dx >= 0) { vel.x = 5; }
            else { vel.x = -5; }

            ball->body->body->SetLinearVelocity(vel);
        }
    }
}

QPointF getNearestPosition()
{
    qreal x, y, x1, y1, x2, y2, length, tmpLength;
    bool isInitialize;

    isInitialize = true;

    Q_FOREACH (OGBall* ball, _balls)
    {
        if (!ball->active)
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

bool testWalkable(OGPhysicsBody* body)
{
    OGStaticBody* data;

    b2ContactEdge* edge = body->body->GetContactList();

    while (edge)
    {
        data = static_cast<OGStaticBody*>(edge->other->GetUserData());

        if (data)
        {
            if (data->tag == "walkable") { return true; }
        }

        edge = edge->next;
    }

    return false;
}
