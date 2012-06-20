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
#include "flags.h"
#include "mainwidget.h"

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";



int main(int argc, char *argv[])
{
    #ifndef Q_OS_WIN32
    BackTracer(SIGSEGV);
    BackTracer(SIGFPE);
    BackTracer(SIGSTKFLT);
    #endif

    //intialize randseed
    qsrand(QTime::currentTime().toString("hhmmsszzz").toUInt());

    //Check for the run parameters
    for (int i=1; i<argc; i++) {
        QString arg(argv[i]);
        //Check for debug Option
        if (!arg.compare("-debug", Qt::CaseInsensitive)) {
            flag |= DEBUG;
            qWarning("DEBUG MODE ON");
        }
        else if (!arg.compare("-opengl", Qt::CaseInsensitive)) {
            flag |= OPENGL;
        }
        else if (!arg.compare("-text", Qt::CaseInsensitive)) {
            flag |= ONLYTEXT | DEBUG;
        }
    }
    if (flag == STANDARD) qWarning("STD MODE");
    if (flag & OPENGL) {
        qWarning("OPENGL ACTIVATED");
        argc += 2; // TODO: check if it's valid
        argv[argc-2] = strdup("-graphicssystem");
        argv[argc-1] = strdup("opengl");
    }


    QApplication a(argc, argv);

    //CHECK FOR GAME DIR IN HOME DIRECTORY
    QDir dir(GAMEDIR);
    //If the game dir doesn't exist create it
    if (!dir.exists()) {
        if (flag & DEBUG) qWarning() << "Game dir doesn't exist!";
        dir.mkdir(GAMEDIR);
        dir.cd(GAMEDIR);
        //create subdir for user levels and progressions.
        dir.mkdir("userLevels");
        dir.mkdir("userProgression");
        dir.mkdir("debug");
    }
    else if (flag & DEBUG) qWarning() << "Game dir exist!";

    MainWidget w(QRect(50, 50, 800, 600));
    w.show();
    return a.exec();
}
