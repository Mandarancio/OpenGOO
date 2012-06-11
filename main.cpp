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


#include <QtGui/QApplication>
#include "mainwidget.h"

#include <QDir>

#include <QRect>
#include <QDesktopWidget>

#include <QDebug>

#include <QGraphicsBlurEffect>
#include <QTime>

#include "backtracer.h"

#define GAMEDIR "/.OpenGOO/"


BackTracer segvTracer(SIGSEGV);
BackTracer fpeTracer(SIGFPE);
BackTracer stkTracer(SIGSTKFLT);

int main(int argc, char *argv[])
{


     //intialize randseed
    qsrand(QTime().currentTime().toString("hh:mm:ss.zzz").remove(':').toFloat());

    bool debug=false;
    bool forceScreen=false;
    bool forceOpenGL=false;
    //Default is screen 0
    int screen=0;
    QString arg;
    //Check for the run parameters
    for (int i=1;i<argc;i++){
        arg=QString::fromAscii(argv[i]);
        //Check for debug Option
        if (!arg.compare("-Debug",Qt::CaseInsensitive)){
            debug=true;
            qWarning("DEBUG MODE ON");
        }
        //Check for screen force option
        else if (!arg.split('=').at(0).compare("-Screen",Qt::CaseInsensitive)){
            screen=arg.split('=').at(1).toInt(&forceScreen);
        }
        else if (!arg.compare("-opengl",Qt::CaseInsensitive)){
            forceOpenGL=true;
        }
    }
    if (!debug ) qWarning("STD MODE");
    if (forceOpenGL){ qWarning("OPENGL ACTIVATED");
        argc+=2;
        argv[argc-2]="-graphicssystem";
        argv[argc-1]="opengl";
    }
    QApplication a(argc, argv);

    if (screen>a.desktop()->numScreens()-1){
        qWarning()<<"Screen"<<screen<<"not found!";
        screen=0;
        forceScreen=false;
    }


    //screenGeometry() return the geometry of the display
    //Algorithm to select the bigger screen in a multi screen configuration
    //If is found a multi screen configuration
    if (!forceScreen){
        screen=0;
        if (a.desktop()->numScreens()>1){
            //compute the first screen area
            int area=a.desktop()->screenGeometry(screen).size().width()*a.desktop()->screenGeometry(screen).height();
            //check all the other screen and find the bigger one!
            for (int i=1;i<a.desktop()->numScreens();i++){
                if (a.desktop()->screenGeometry(i).size().width()*a.desktop()->screenGeometry(i).height() > area){
                    screen=i;
                    area=a.desktop()->screenGeometry(screen).size().width()*a.desktop()->screenGeometry(screen).height();
                }
            }
        }
    }

    //CHECK FOR GAME DIR IN HOME DIRECTORY
    QString homePath=QDir::homePath();
    QString gameDir=GAMEDIR;
    QDir dir;
    dir.setPath(homePath+gameDir);
    //If the game dir doesn't exist create it
    if (!dir.exists()){
        if (debug) qWarning()<<"Game dir doesn't exist!";
        dir.mkdir(homePath+gameDir);
        dir.cd(homePath+gameDir);
        //create subdir for user levels and progressions.
        dir.mkdir("userLevels/");
        dir.mkdir("userProgression/");
        dir.mkdir("debug/");
    }
    else if (debug) qWarning()<<"Game dir exist!";
    //Create the main widget in the bigger screen
    MainWidget w(a.desktop()->screenGeometry(screen),debug);
    w.show();

    return a.exec();
}
