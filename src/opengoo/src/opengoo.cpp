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

    OGConfig config;
    // Default settings
    config.fullscreen = true;
    config.screen_width = 800;
    config.screen_height = 600;

    // Read game configuration
    QString filename("./resources/config.xml");
    OGGameConfig gameConfig(filename);

    if (gameConfig.Open())
    {
        if (gameConfig.Read())
        {
            gameConfig.Parser(config);
        }
        else {logWarn("File " + filename + " is corrupted"); }
    }
    else
    {
        logWarn("File " + filename +" not found");
        gameConfig.Create(config);
    }

#ifdef Q_OS_WIN32
    _gameEngine = new OGGameEngine(config.screen_width, config.screen_height, config.fullscreen);
#else
    _gameEngine = new OGGameEngine(config.screen_width, config.screen_height, false);
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

    //initialize video mode
#ifdef Q_OS_WIN32
        _vm = OGVideoMode::getCurrentMode();
        _isVideoModeSupported = OGVideoMode::testVideoMode(_gameEngine->getWidth(), _gameEngine->getHeight());
        if (_isVideoModeSupported)
        {
            OGVideoMode::setVideoMode(_gameEngine->getWidth(), _gameEngine->getHeight());
        }
#endif // Q_OS_WIN32

    // Read resources
    QString filename("./res/levels/MapWorldView/MapWorldView.resrc.xml");
    OGResourceConfig resConfig(filename);

    if (resConfig.Open())
    {
        if (resConfig.Read())
        {
            resConfig.Parser(_resources);
        }
        else {logWarn("File " + filename + " is corrupted"); }
    }
    else { logWarn("File " + filename +" not found"); }

    // Read text
    filename = "./properties/text.xml";
    OGTextConfig textConfig(filename);

    if (textConfig.Open())
    {
        if (textConfig.Read())
        {
            textConfig.Parser(_strings);
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
            sceneConfig.Parser(_scene);
        }
        else {logWarn("File " + filename + " is corrupted"); }
    }
    else { logWarn("File " + filename +" not found"); }

    _isMainMenu = true;
    _isMainMenuInitialize = false;

    //initialize camera
    float posY = _scene.maxy - 300.0 - 600.0 * 0.5;
    float posX = -(qAbs(_scene.minx) + -64.5 - 800.0 * 0.5);

    _camera.setX(posX);
    _camera.setY(posY);
    _camera.setSize(QSize(_gameEngine->getWidth(), _gameEngine->getHeight()));
}

void GameEnd()
{
#ifdef Q_OS_WIN32
    if (_isVideoModeSupported)
    {
        OGVideoMode::setVideoMode(_vm.width(), _vm.height());
    }
#endif // Q_OS_WIN32
    delete _gameEngine;
}

void GameCycle()
{
    _gameEngine->getWindow()->render();
}

void GamePaint(QPainter *painter)
{
    // Test
    QTransform transform;
    transform.translate(_camera.x(), _camera.y());

    painter->setWorldTransform(transform);

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
    OGWindow* window = _gameEngine->getWindow();

    glClearColor(_scene.backgroundcolor.red()
                 , _scene.backgroundcolor.green()
                 , _scene.backgroundcolor.blue(), 1);

    glClear(GL_COLOR_BUFFER_BIT);

    if (!_isMainMenuInitialize)
    {
        for (int i=0; i<_scene.sceneLayers.size(); i++)
        {
            QString image(_scene.sceneLayers.at(i).image);
            qreal sx = _scene.sceneLayers.at(i).scalex;
            qreal sy = _scene.sceneLayers.at(i).scaley;

            QImage sprite(GetResource(OGResource::IMAGE, image) +".png");

            QTransform t;
            t.scale(sx, sy);

            _resImages << QPixmap::fromImage(sprite.transformed(t));
        }

        _isMainMenuInitialize = true;
    }

    for (int i=0; i<_scene.sceneLayers.size(); i++)
    {
        qreal x = _scene.sceneLayers.at(i).x;
        qreal y = _scene.sceneLayers.at(i).y;

        qreal offX= (qAbs(_scene.minx) + x) - _resImages.at(i).width() * 0.5;
        qreal offY= (_scene.maxy - y) - _resImages.at(i).height() * 0.5;

        painter->drawPixmap(QPointF(offX, offY), _resImages.at(i));
    }

    for (int i=0; i<_strings.size(); i++)
    {
        QString text = _strings.at(i).text;
        painter->setPen(Qt::yellow);
        painter->setFont(QFont("Arial", 30));
        painter->drawText(QRect(QPoint(0,0), window->size()),  Qt::AlignCenter, text);
    }
}

QString GetResource(OGResource::Type type, QString id)
{
    for (int i=0; i<_resources.size(); i++)
    {
        if (_resources.at(i).type == type)
        {
            if (_resources.at(i).id == id)
            {
                return _resources.at(i).path;
            }
        }
    }

    return QString();
}
