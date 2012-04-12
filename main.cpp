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

#include <QRect>
#include <QDesktopWidget>

#include <QDebug>

#include <QTime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
     //intialize randseed
    qsrand(QTime().currentTime().toString("hh:mm:ss.zzz").remove(':').toFloat());

    bool debug=false;
    bool forceScreen=false;
    bool multiwindow=false;
    //Default is screen 0
    int screen=0;
    QString arg;
    //Check for the run parameters
    for (int i=1;i<argc;i++){
        arg=QString::fromAscii(argv[i]);
        //Check for debug Option
        if (!arg.compare("-Debug")){
            debug=true;
            qWarning("DEBUG MODE ON");
        }
        //Check for screen force option
        else if (!arg.split('=').at(0).compare("-Screen")){
            screen=arg.split('=').at(1).toInt(&forceScreen);
            if (screen>a.desktop()->numScreens()-1){
                qWarning()<<"Screen"<<screen<<"not found!";
                screen=0;
                forceScreen=false;
            }
        }
        else if (!arg.compare("-Multiwindow")){
                  multiwindow=true;
        }
    }
    if (!debug) qWarning("STD MODE");
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
    //Create the main widget in the bigger screen
    MainWidget w(a.desktop()->screenGeometry(screen),debug,multiwindow);
    w.show();

    return a.exec();
}
