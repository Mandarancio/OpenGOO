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

#ifndef Q_OS_WIN32
#include "backtracer.h"
#endif
#ifdef Q_OS_WIN32
#include "backtracer_win32.h"
#endif

#include "flags.h"

#include "soundsystem.h"

#include <logger.h>
#include <consoleappender.h>

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
        else
        {
            _levelname = arg;
        }
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

#ifdef Q_OS_WIN32
    _gameEngine =
            new OGGameEngine(
                _config.screen_width,
                _config.screen_height,
                _config.fullscreen
                );
#else
    _gameEngine =
            new OGGameEngine(
                _config.screen_width,
                _config.screen_height,
                false
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

    // Read text
    QString filename = "./properties/text.xml";
    OGTextConfig textConfig(filename);

    if (textConfig.Open())
    {
        if (textConfig.Read())
        {
            _strings = textConfig.Parser();
        }
        else {logWarn("File " + filename + " is corrupted"); }
    }
    else { logWarn("File " + filename +" not found"); }  

    _isLevelInitialize = false;

    // Load main menu
     if(_levelname.isEmpty()) { _levelname = "MapWorldView"; }

    // Default camera settings
    _camera.position = OGPosition(); // (0,0)
}

void GameEnd()
{
    delete _gameEngine;
}

void GameCycle()
{
    scroll();
    _gameEngine->getWindow()->render();
}

void GamePaint(QPainter *painter)
{
    // Test

    if (!_isLevelInitialize)
    {
        createScene(_levelname);
        _isLevelInitialize = true;
    }

    // Set background
    glClearColor(
                _scene.backgroundcolor.red(),
                _scene.backgroundcolor.green(),
                _scene.backgroundcolor.blue(),
                1
                );

    glClear(GL_COLOR_BUFFER_BIT);

    qreal w = _gameEngine->getWidth() / _camera.zoom;
    qreal h = _gameEngine->getHeight() / _camera.zoom;
    qreal x = (_camera.position.x() - w * 0.5);
    qreal y = (_camera.position.y() + h * 0.5)*(-1);

    painter->setWindow(x, y, w, h);

    painter->setRenderHint(QPainter::HighQualityAntialiasing);

    // Draw scene
    for (int i=0; i< _resSprites.size(); i++)
    {
        painter->setOpacity(_resSprites.at(i).opacity);
        qreal x = _resSprites.at(i).position.x();
        qreal y = _resSprites.at(i).position.y();
        qreal w = _resSprites.at(i).size.width();
        qreal h = _resSprites.at(i).size.height();
        painter->drawPixmap(
                    QRectF(x, y, w, h)
                    , _resSprites.at(i).sprite
                    , _resSprites.at(i).sprite.rect()
                    );
    }

    painter->setOpacity(1);

}

void GameActivate()
{
}

void GameDeactivate()
{
}

void HandleKeys(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Left:        
        break;

    case Qt::Key_Right:        
        break;

    case Qt::Key_Up:        
        break;

    case Qt::Key_Down:       
        break;

    case Qt::Key_Escape:
        _gameEngine->gameExit();
        break;
    }
}

void MouseButtonDown(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void MouseButtonUp(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void MouseMove(QMouseEvent* event)
{
    Q_UNUSED(event)

    qreal x = event->pos().x();
    qreal y = event->pos().y();
    Scroll scroll = {false, false, false, false};
    _scroll = scroll;
    const qreal OFFSET = 50.0;

    if (x <= OFFSET)
    {
         _scroll.left = true;
    }
    else if (x >= _gameEngine->getWidth() - OFFSET)
    {
        _scroll.right = true;
    }

    if (y <= OFFSET)
    {
        _scroll.up = true;
    }
    else if (y >= _gameEngine->getHeight() - OFFSET)
    {
        _scroll.down = true;
    }
}

void MouseWheel(QWheelEvent* event)
{
    QPoint numDegrees = event->angleDelta() / 8;

    if (numDegrees.y() > 0) {
        zoom(-1);
    }
    else { zoom(1); }

    event->accept();
}

void gooMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
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

void createScene(const QString & levelname)
{    
    readGameConfiguration(levelname);

    // Create scene
    for (int i=0; i<_scene.sceneLayer.size(); i++)
    {
        OGSceneLayer scene = _scene.sceneLayer.at(i);
        // Create sprite
        OGSprite sprite;

        sprite.color = scene.colorize;
        sprite.scale= scene.scale;

        sprite.opacity = scene.alpha;
        sprite.rotation = scene.rotation;
        sprite.depth = scene.depth;

        createSprite(&sprite, scene.image);

        // Set sprite size
        qreal width = sprite.sprite.width() * sprite.scale.x();
        qreal height = sprite.sprite.height() * sprite.scale.y();
        sprite.size = QSize(width, height);

        // Set sprite position
        OGPosition pos = scene.position;
        qreal x = pos.x() - sprite.size.width() * 0.5;
        qreal y = (pos.y() + sprite.size.height() * 0.5)*(-1.0);
        sprite.position = OGPosition(x, y);

        // Put sprite into list
        _resSprites << sprite;
    }

    // Create buttongroups
    for (int i=0; i<_scene.buttongroup.size(); i++)
    {
        OGButtonGroup btnGroup = _scene.buttongroup.at(i);

        // Create buttons
        for (int j=0; j<btnGroup.button.size(); j++)
        {
            createButton(btnGroup.button.at(j), UP);
        }
    }

    // Create buttons
    for (int i=0; i<_scene.button.size(); i++)
    {
        createButton(_scene.button.at(i), UP);
    }

    // Create Z-order
    // Bubble sort
    // Source http://en.wikibooks.org/wiki/Algorithm_Implementation/Sorting/Bubble_sort#C.2B.2B
    QList <OGSprite>::iterator first = _resSprites.begin();
    QList <OGSprite>::iterator last = _resSprites.end();
    QList <OGSprite>::iterator i;

    while(first < --last)
    {
        for(i=first; i < last; ++i)
        {
           if ( (i+1)->depth < i->depth)
           {
               std::iter_swap( i, i+1 );
           }
        }
    }

    //initialize camera
    for (int i=0; i < _level.camera.size(); i++)
    {
        if (_level.camera.at(i).aspect == "normal")
        {
            int last = _level.camera.at(i).poi.size()-1;
            _camera.position = _level.camera.at(i).poi.at(last).position;
            _camera.zoom = _level.camera.at(i).poi.at(last).zoom;
            break;
        }
    }
}

void createSprite(OGSprite* sprite, const QString & image)
{
    QImage source(_resources.GetResource(OGResource::IMAGE, image) +".png");

    QTransform transform;
    transform.rotate(-sprite->rotation);

    QImage tmpImage(
                source.transformed(transform, Qt::SmoothTransformation)
                );

    QImage target(tmpImage.size(), QImage::Format_ARGB32_Premultiplied);

    // Colorize sprite
    QPainter painter(&target);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(target.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(target.rect(), tmpImage, tmpImage.rect());

    if (!source.hasAlphaChannel())
    {
        painter.setCompositionMode(QPainter::CompositionMode_Multiply);
        painter.fillRect(target.rect(), sprite->color);
    }
    else
    {
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(target.rect(), sprite->color);
        painter.setCompositionMode(QPainter::CompositionMode_Multiply);
        painter.drawImage(target.rect(), tmpImage, tmpImage.rect());
    }

    painter.end();

    sprite->sprite = QPixmap::fromImage(target);
}

void readGameConfiguration(const QString & level)
{
    // Read resources
    QString filename("./res/levels/" + level + "/" + level);
    OGResourceConfig resConfig(filename + ".resrc.xml");

    if (resConfig.Open())
    {
        if (resConfig.Read())
        {            
            _resources = resConfig.Parser();
        }
        else {logWarn("File " + filename + " is corrupted"); }
    }
    else { logWarn("File " + filename +" not found"); }

    // Read level
//    filename = "./res/levels/MapWorldView/MapWorldView.level.xml";
    OGLevelConfig levelConfig(filename + ".level.xml");

    if (levelConfig.Open())
    {
        if (levelConfig.Read())
        {
            _level = levelConfig.Parser();
        }
        else {logWarn("File " + filename + " is corrupted"); }
    }
    else { logWarn("File " + filename +" not found"); }

    // Read scene
//    filename = "./res/levels/MapWorldView/MapWorldView.scene.xml";
    OGSceneConfig sceneConfig(filename + ".scene.xml");

    if (sceneConfig.Open())
    {
        if (sceneConfig.Read())
        {
            _scene = sceneConfig.Parser();
        }
        else {logWarn("File " + filename + " is corrupted"); }
    }
    else { logWarn("File " + filename +" not found"); }
}

void createButton(const OGButton & button, OGAction action )
{
    // Create sprite
    OGSprite sprite;

    sprite.scale = button.scale;
    sprite.rotation = button.rotation;
    sprite.color = button.colorize;
    sprite.opacity = button.alpha;
    sprite.depth = button.depth;

    QString img;
    switch (action)
    {
    case UP:
        img = button.up;
        break;
    case OVER:
        img = button.over;
        break;
    case DISABLED:
        img = button.disabled;
        break;
    }

    createSprite(&sprite, img);

    // Set sprite size
    qreal width = sprite.sprite.width() * sprite.scale.x();
    qreal height = sprite.sprite.height() * sprite.scale.y();
    sprite.size = QSize(width, height);

    // Set sprite position
    OGPosition pos = button.position;
    qreal x = pos.x() - sprite.size.width() * 0.5;
    qreal y = (pos.y() + sprite.size.height() * 0.5)*(-1);
    sprite.position = OGPosition(x, y);

    _resSprites << sprite;
}

void scroll()
{
    const qreal SHIFT = 10;

    qreal w = _gameEngine->getWidth() / _camera.zoom;
    qreal h = _gameEngine->getHeight() / _camera.zoom;

    if (_scroll.up)
    {
        qreal pos = _camera.position.y()+SHIFT;

        if (pos <= _scene.maxy - h*0.5)
        {
            _camera.position.setY(pos);
        }
    }
    else if (_scroll.down)
    {
        qreal pos = _camera.position.y()-SHIFT;

        if (pos >= _scene.miny + h*0.5)
        {
            _camera.position.setY(pos);
        }
    }

    if (_scroll.left)
    {
        qreal pos = _camera.position.x()-SHIFT;

        if (pos >= _scene.minx + w*0.5)
        {
            _camera.position.setX(pos);
        }
    }
    else if (_scroll.right)
    {
        qreal pos = (_camera.position.x()+SHIFT);

        if (pos <= _scene.maxx - w*0.5)
        {
            _camera.position.setX(pos);
        }
    }
}

void zoom(int direct)
{
    qreal zoom = _camera.zoom  + (0.1 * direct);

    if (
            (_scene.maxx+qAbs(_scene.minx))*zoom >= _gameEngine->getWidth()
            &&
            (_scene.maxy+qAbs(_scene.miny))*zoom >= _gameEngine->getHeight()
            )
    {
        _camera.zoom = zoom;
    }
}
