#include "menu.h"
#include "flags.h"

#include <QColor>
#include <QTextStream>
#include <QFile>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <logger.h>

Menu::Menu(QRect geometry, QObject *parent) :
    QObject(parent)
{
    this->geometry=geometry;
    computeHeight();

    loadMenuFile();
    selected=0;
}

void Menu::paint(QPainter &p){
    QColor bg(0,0,0,200);
    p.setPen(Qt::transparent);
    p.setBrush(bg);
    p.drawRect(0,0,geometry.width(),geometry.height());
    p.setPen(Qt::white);
    p.setBrush(Qt::white);
    for (int i=0;i<index.length();i++)
        paintButton(i,p);
    if(flag & DEBUG) p.drawRect(geometry.width()/2-1,geometry.height()/2-1,2,2);//Draw center of the screen
}

void Menu::loadMenuFile(){
    QFile file("./resources/menu.index");
    if (file.open(QFile::ReadOnly)){
        QTextStream in(&file);
        QString tmp;
        int i=0;
        while(!in.atEnd()){
            tmp=in.readLine();
            if (tmp.at(0)=='#') continue;//comment!
            else if (tmp.at(0)=='-') {
                tmp.remove(0,1);
                index.push_back(tmp);
                i++;
            }
        }
        int yOffset=(geometry.height()-index.size()*(height+30)+30)/2;
        for(int j=0;j<index.size();j++)
        {
            int y=yOffset+(height+30)*j;
            buttons.push_back(QRect(geometry.width()/4,y,geometry.width()/2,height));
        }


        file.close();
    }
    else logWarn("Menu file dosn't exist!");
}

void Menu::paintButton(int ind, QPainter &p){

    QFont f;
    f.setFamily("Times");
    f.setPointSize(32);
    p.setFont(f);
    QColor bBg;//(255,255,255,30);
    bBg.setRgb(0,0,0,100);
    p.setBrush(bBg);
    p.setPen(Qt::darkGray);
    p.drawRoundedRect(buttons.at(ind),15,15);
    p.setPen(Qt::white);
    p.drawText(buttons.at(ind),Qt::AlignCenter,index.at(ind));
}

void Menu::computeHeight(){
    QFont f;
    f.setFamily("Times");
    f.setPointSize(32);
    height=QFontMetrics(f).height()+30;
}

void Menu::mouseRelease(QMouseEvent *e){
    if (e->button()==Qt::LeftButton){
        int index=-1;
        for (int i=0;i<buttons.length();i++){
            if (buttons[i].contains(e->pos())) index=i;
        }
        switch(index){
        case(-1):
            return;
            break;
        case (0):
            emit this->eventResume();
            break;
        case(1):
            emit this->eventRestart();
            break;
        case(2):
            emit this->eventBackToMainMenu();
            break;
        case(3):
            emit this->eventClose();
            logWarn("Esci!");
            break;
//        case(4):
//            logWarn() << "Opzioni!";
//            break;
        default:
            return;
            break;
        }
    }
}


void Menu::setGeometry(QRect geometry){
    this->geometry=geometry;
    int yOffset=(geometry.height()-buttons.length()*(height+30)+30)/2;
    for(int j=0;j<buttons.length();j++)
    {
        int y=yOffset+(height+30)*j;
        buttons[j]=QRect(geometry.width()/4,y,geometry.width()/2,height);
    }
}
