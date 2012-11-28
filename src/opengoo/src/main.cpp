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

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
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
#include "mainwidget.h"

#include "soundsystem.h"
#include "publicclass.h"

#include <logger.h>
#include <consoleappender.h>

struct OGGameConfig
{
    int screen_width;
    int screen_height;
    QString language;
};

void gooMessageOutput(QtMsgType type, const char *msg);
void ReadConfig(OGGameConfig* config);

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";

int main(int argc, char *argv[])
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
    qInstallMsgHandler(gooMessageOutput);

    //intialize randseed
    qsrand(QTime::currentTime().toString("hhmmsszzz").toUInt());

    //Check for the run parameters
    for (int i=1; i<argc; i++) {
        QString arg(argv[i]);
        //Check for debug Option
        if (!arg.compare("-debug", Qt::CaseInsensitive)) {
            flag |= DEBUG;
            logWarn("DEBUG MODE ON");
        }
        else if (!arg.compare("-opengl", Qt::CaseInsensitive)) {
            flag |= OPENGL | FPS;
        }
        else if (!arg.compare("-text", Qt::CaseInsensitive)) {
            flag |= ONLYTEXT | DEBUG;
        }
        else if (!arg.compare("-fps",Qt::CaseInsensitive)){
            flag |= FPS;
        }
    }
    if (flag == STANDARD) logWarn("STD MODE");
    if (flag & OPENGL) {
        logWarn("OPENGL ACTIVATED");
        QApplication::setGraphicsSystem("opengl");
    }


    QApplication a(argc, argv);

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

    OGGameConfig config;

    ReadConfig(&config);

    MainWidget w(QRect(50, 50, config.screen_width, config.screen_height));
    w.show();
    return a.exec();
}

void gooMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        logDebug(QLatin1String(msg));
        break;
    case QtWarningMsg:
        logWarn(QLatin1String(msg));
        break;
    case QtCriticalMsg:
        logCritical(QLatin1String(msg));
        break;
    case QtFatalMsg:
        logException(QLatin1String(msg));
        abort();
    }
}

void ReadConfig(OGGameConfig* config)
{
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGTH = 600;
    QDomDocument domDoc;
    QFile file("./resources/config.xml");

    if (file.open(QIODevice::ReadOnly))
    {
        if (domDoc.setContent(&file))
        {
            QDomElement domElement = domDoc.documentElement();
            if (domElement.tagName() != "config")
            {
                config->screen_width = SCREEN_WIDTH;
                config->screen_height = SCREEN_HEIGTH;
            }

            for(QDomNode n = domElement.firstChild(); !n.isNull(); n = n.nextSibling())
             {
                 QDomElement domElement = n.toElement();

                 if (domElement.tagName() == "param")
                 {
                     QString attribute = domElement.attribute("name", "");

                     if (attribute == "screen_width")
                     {
                         config->screen_width = domElement.attribute("value", "").toInt();
                     }
                     else if (attribute == "screen_height")
                     {
                         config->screen_height = domElement.attribute("value", "").toInt();
                     }
                     else if (attribute == "language")
                     {
                         config->language = domElement.attribute("value", "");
                     }
                 }
             }
        }

        file.close();
    }

    else
    {
        gooMessageOutput(QtWarningMsg, "File config.xml not found");
        config->screen_width = SCREEN_WIDTH;
        config->screen_height = SCREEN_HEIGTH;
    }
}
