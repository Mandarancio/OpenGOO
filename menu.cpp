#include "menu.h"
#include <QColor>

Menu::Menu(QRect geometry, QObject *parent) :
    QObject(parent)
{
    this->geometry=geometry;
}

void Menu::paint(QPainter &p){
    QColor bg(0,0,0,200);
    p.setPen(Qt::transparent);
    p.setBrush(bg);
    p.drawRect(0,0,geometry.width(),geometry.height());
}
