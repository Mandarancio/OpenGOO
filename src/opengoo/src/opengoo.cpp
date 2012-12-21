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
    
    _gameEngine = new OGGameEngine(config.screen_width, config.screen_height, config.fullscreen);

    if (_gameEngine == 0) { return false; }

    _gameEngine->setFrameRate(60);

    return true;
}

void GameStart()
{
    //intialize randseed
    qsrand(QTime::currentTime().toString("hhmmsszzz").toUInt());
    _gameEngine->getWindow()->setCursor(Qt::BlankCursor);

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

    _isMainMenu = true;
}

void GameEnd()
{
    delete _gameEngine;
}

void GameCycle()
{

}

void GamePaint(QPainter *painter)
{
    // Test
    OGWindow* window = _gameEngine->getWindow();
    if (_isMainMenu)
    {
        mainMenu(painter);
    }
    QString text("Press escape to exit  full screen mode");
    painter->setPen(Qt::yellow);
    painter->setFont(QFont("Arial", 30));
    painter->drawText(QRect(QPoint(), window->size()),  Qt::AlignCenter, text);
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

    for (int i=0; i<_resources.size(); i++)
    {
        if (_resources.at(i).type == OGResource::IMAGE)
        {
            QPixmap sprite(_resources.at(i).path +".png");
            painter->drawPixmap(QRect(QPoint(), window->size()) , sprite, sprite.rect());
        }
    }
}

