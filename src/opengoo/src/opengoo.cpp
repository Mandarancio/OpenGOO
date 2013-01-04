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
    _gameEngine->getWindow()->setCursor(Qt::BlankCursor);

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

    _isMainMenu = true;
    _isMainMenuInitialize = false;

    // Default camera settings
    _camera.setX(0);
    _camera.setY(0);

    _camera.setSize(
                QSize(_gameEngine->getWidth(), _gameEngine->getHeight())
                );
}

void GameEnd()
{
    delete _gameEngine;
}

void GameCycle()
{
    _gameEngine->getWindow()->render();
}

void GamePaint(QPainter *painter)
{
    QTransform transform;
    transform.translate(_camera.x(), _camera.y());
    painter->setWorldTransform(transform);
    painter->setRenderHint(QPainter::HighQualityAntialiasing);

    // Test
    if (_isMainMenu)
    {
        mainMenu(painter);
    }
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
        _camera.setX(_camera.x()-1);
        break;

    case Qt::Key_Right:
        _camera.setX(_camera.x()+1);
        break;

    case Qt::Key_Up:
        _camera.setY(_camera.y()-1);
        break;

    case Qt::Key_Down:
        _camera.setY(_camera.y()+1);
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

void mainMenu(QPainter* painter)
{    
    if (!_isMainMenuInitialize)
    {
        readGameConfiguration();

        // Create scene
        for (int i=0; i<_scene.sceneLayer.size(); i++)
        {
            // Create sprite
            OGSprite sprite;

            sprite.color = _scene.sceneLayer.at(i).colorize;
            sprite.scale= _scene.sceneLayer.at(i).scale;

            sprite.opacity = _scene.sceneLayer.at(i).alpha;
            sprite.rotation = _scene.sceneLayer.at(i).rotation;
            sprite.depth = _scene.sceneLayer.at(i).depth;

            createSprite(&sprite, _scene.sceneLayer.at(i).image);

            // Set sprite position
            OGPosition pos = _scene.sceneLayer.at(i).position;
            qreal width = sprite.sprite.width();
            qreal height = sprite.sprite.height();            
            sprite.pos.setX((qAbs(_scene.minx) + pos.x()) - width * 0.5);
            sprite.pos.setY((_scene.maxy - pos.y()) - height * 0.5);

            // Put sprite into list
            _resSprites << sprite;
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
        qreal camX = _level.camera.at(0).poi.at(0).position.x();
        qreal camY = _level.camera.at(0).poi.at(0).position.y();
        qreal width = _gameEngine->getWidth();
        qreal height = _gameEngine->getHeight();
        qreal posX = -(qAbs(_scene.minx) + camX - width * 0.5);
        qreal posY = _scene.maxy - camY - height * 0.5;
        _camera.setX(posX);
        _camera.setY(posY);
        _camera.setSize(QSize(width, height));

        _isMainMenuInitialize = true;
    }

    // Set background
    glClearColor(
                _scene.backgroundcolor.red(),
                _scene.backgroundcolor.green(),
                _scene.backgroundcolor.blue(),
                1
                );

    glClear(GL_COLOR_BUFFER_BIT);

    // Draw scene
    for (int i=0; i< _scene.sceneLayer.size(); i++)
    {
        painter->setOpacity(_resSprites.at(i).opacity);
        painter->drawPixmap(_resSprites.at(i).pos, _resSprites.at(i).sprite);
    }
}

void createSprite(OGSprite* sprite, const QString & image)
{
    QImage source(_resources.GetResource(OGResource::IMAGE, image) +".png");

    QTransform transform;
    transform.scale(sprite->scale.x(), sprite->scale.y());
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

void readGameConfiguration()
{
    // Read resources
    QString filename("./res/levels/MapWorldView/MapWorldView.resrc.xml");
    OGResourceConfig resConfig(filename);

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
    filename = "./res/levels/MapWorldView/MapWorldView.level.xml";
    OGLevelConfig levelConfig(filename);

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
    filename = "./res/levels/MapWorldView/MapWorldView.scene.xml";
    OGSceneConfig sceneConfig(filename);

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
