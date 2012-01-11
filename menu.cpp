#include "menu.h"
#include <QColor>
#include <QTextStream>
#include <QFile>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>

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
}

void Menu::loadMenuFile(){
    QFile file("menu.index");
    if (file.open(QFile::ReadOnly)){
        QTextStream in(&file);
        QString tmp;
        int y;
        int i=0;
        while(!in.atEnd()){
            tmp=in.readLine();
            if (tmp.at(0)=='#') continue;//comment!
            else if (tmp.at(0)=='-') {
                tmp.remove(0,1);
                y=(height+30)*i+30;
                buttons.push_back(QRect(geometry.width()/2-30,y,geometry.width()/2,height));
                index.push_back(tmp);
                i++;
            }
        }

        file.close();
    }
    else qWarning("Menu file dosn't exist!");
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
            emit this->eventClose();
            break;
        default:
            return;
            break;
        }
    }
}
