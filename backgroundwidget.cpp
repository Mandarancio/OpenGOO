#include "backgroundwidget.h"
#include <QPainter>

BackGroundWidget::BackGroundWidget(QWidget *parent) :
    QWidget(parent)
{
    //SCALE DEFAULT 1
    scale=1.0;
    //TRANSLATION
    translation=QPoint(0,0);
    //SETUP COLOR RADIENT)
    QColor c1,c2;
    c1.setRgb(95,141,211);
    c2.setRgb(21,33,50);
    bgColor=QRadialGradient(QPoint(0,height()/2),2*height());
    bgColor.setColorAt(0,c1);
    bgColor.setColorAt(1,c2);
}

void BackGroundWidget::paintEvent(QPaintEvent *e){
    QPainter p(this);
    p.save();
    p.scale(scale,scale);
    p.translate(translation);

    //DRAW BG COLOR
    p.setPen(Qt::transparent);
    p.setBrush(bgColor);
    p.drawRect(limit.x(),+limit.y(),limit.width(),limit.height());

    //DRAW BACKGROUND IMAGE
    for (int i=0;i<background.length();i++){
        background[i]->setTranslate(translation);
        background[i]->paint(p);
    }
    //DRAW GROUND IMAGE
    for (int i=0;i<ground.length();i++) ground[i]->paint(p);


    p.restore();
    if (p.end()) e->accept();
    else e->ignore();
}

void BackGroundWidget::addBackGround(BackGround *bg){
    if (background.contains(bg)) return;
    else background.push_back(bg);
}

void BackGroundWidget::addGround(Ground *ground){
    if (this->ground.contains(ground)) return;
    else this->ground.push_back(ground);
}

void BackGroundWidget::translated(QPoint p){
    translation=p;
    update();
}

void BackGroundWidget::setScale(float scale){
   this->scale=scale;
}

void BackGroundWidget::setLimit(QRect limit){
    this->limit=limit;
    QColor c1,c2;
    c1.setRgb(95,141,211);
    c2.setRgb(21,33,50);
    bgColor=QRadialGradient(QPoint(0,height()/2),2*height());
    bgColor.setColorAt(0,c1);
    bgColor.setColorAt(1,c2);
}

void BackGroundWidget::clear(){
    background.clear();
    ground.clear();
    scale=1;
    translation=QPoint(0,0);

}
