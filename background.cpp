#include "background.h"
#include <QGraphicsBlurEffect>

BackGround::BackGround(int id,QObject *parent) :
    QObject(parent)
{
    //initialize
    this->id=id;
    delta=0;
    translate=QPoint(0,0);
}

//Add a shape!
void BackGround::addPolygon(QPolygon polygon, QColor color){
    polygons.push_back(QPair<QPolygon,QColor>(polygon,color));
}

///Set delta
void BackGround::setDelta(float delta){
    this->delta=delta;
}

//get id
int BackGround::getID(){
    return id;
}
//Set translate
void BackGround::setTranslate(QPoint p){
    translate=QPoint(-p.x()*delta,-p.y()*delta);
}

//paint
void BackGround::paint(QPainter &p){

    QGraphicsBlurEffect *effect;
    effect=new QGraphicsBlurEffect();
    effect->setBlurRadius(1.5);
    p.save();
    p.translate(translate);
    p.setPen(Qt::transparent);
    for (int i=0;i<polygons.length();i++){
       p.setBrush(polygons[i].second);
       p.drawPolygon(polygons[i].first);
    }
    p.restore();
}
