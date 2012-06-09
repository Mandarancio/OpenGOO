#include "background.h"
#include <QGraphicsBlurEffect>
#include <QGraphicsPolygonItem>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

BackGround::BackGround(int id,QObject *parent) :
    QObject(parent)
{
    //initialize
    this->id=id;
    delta=0;
    translate=QPoint(0,0);
    scene=new QGraphicsScene(-500,0,3000,2000);
    scene->setBackgroundBrush(Qt::transparent);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

}

//Add a shape!
void BackGround::addPolygon(QPolygon polygon, QColor color){
    polygons.push_back(QPair<QPolygon,QColor>(polygon,color));
    QGraphicsPolygonItem * item=new QGraphicsPolygonItem(polygon);
    item->setBrush(color);
    item->setPen(QPen(Qt::transparent,0));
    item->setActive(true);

    QGraphicsBlurEffect *effect=new QGraphicsBlurEffect();
    effect->setBlurRadius(10*1/this->id);
    effect->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
    item->setGraphicsEffect(effect);

    item->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    scene->addItem(item);
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

        QGraphicsItem *item;
        QStyleOptionGraphicsItem option;
        option.state=QStyle::State_Enabled;

        for (int i=0;i<scene->items().length();i++){
            item=scene->items()[i];

            p.save();
            p.setMatrix(scene->items()[i]->sceneMatrix(),true);
//            p.setWorldTransform(item->graphicsEffect()->);
            scene->items()[i]->paint(&p,&option);
            p.restore();
        }
        p.restore();

}
