#include "background.h"
#include <QGraphicsBlurEffect>
#include <QImage>
#include <QGraphicsPolygonItem>
#include <QStyleOptionGraphicsItem>
#include "graphicseffect.h"

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

QRect BackGround::computeRect(){
    QRectF rect=polygons[0].first.boundingRect();
    for (int i=1;i<polygons.length();i++){
        rect.unite(polygons[i].first.boundingRect());
    }
    return rect.toRect();
}

//Set translate
void BackGround::setTranslate(QPoint p){
    translate=QPoint(-p.x()*delta,-p.y()*delta);
}

//paint
void BackGround::paint(QPainter &p){

        p.save();
        p.translate(translate);
        p.setPen(Qt::transparent);
        QStyleOptionGraphicsItem * option=new QStyleOptionGraphicsItem();
        for (int i=0;i<polygons.length();i++){
//           p.setBrush(polygons[i].second);
//           p.drawPolygon(polygons[i].first);
            QGraphicsPolygonItem item(polygons[i].first);
            item.setBrush(polygons[i].second);
            item.setPen(QPen(Qt::transparent));
            QGraphicsBlurEffect * effect=new QGraphicsBlurEffect();
            effect->setBlurRadius(5);

            item.setGraphicsEffect(effect);
            item.paint(&p,option);
        }
        p.restore();

}
