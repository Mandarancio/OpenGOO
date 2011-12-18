#include "menu.h"
#include <QColor>
#include <QTextStream>
#include <QFile>
#include <QFont>

#include <QDebug>

Menu::Menu(QRect geometry, QObject *parent) :
    QObject(parent)
{
    this->geometry=geometry;

    loadMenuFile();
    computeHeight();
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
        paintButton(i,index[i],p);
}

void Menu::loadMenuFile(){
    QFile file("menu.index");
    if (file.open(QFile::ReadOnly)){
        QTextStream in(&file);
        QString tmp;
        while(!in.atEnd()){
            tmp=in.readLine();
            if (tmp.at(0)=='#') continue;//comment!
            else if (tmp.at(0)=='-') {
                tmp.remove(0,1);

                index.push_back(tmp);
            }
        }

        file.close();
    }
    else qWarning("Menu file dosn't exist!");
}

void Menu::paintButton(int ind, QString text, QPainter &p){
    int y=10+ind*(height);
    QRect r(0,y,geometry.width(),height-10);
    QFont f;
    f.setFamily("Times");
    f.setPointSize(32);
    p.setFont(f);
    p.drawText(r,Qt::AlignCenter,text);
}

void Menu::computeHeight(){
    int h=geometry.height();
    int t=index.length();
    height=float(h)/((t));
}
