#include "background.h"
#include <QGraphicsBlurEffect>
#include <QGraphicsPolygonItem>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsEffect>
#include <QGraphicsItem>

#define Q_D(Class) Class##Private * const d = d_func()

BackGround::BackGround(int id,QObject *parent) :
    QObject(parent)
{
    //initialize
    this->id=id;
    delta=0;
    translate=QPoint(0,0);
    scene=NULL;
    img=NULL;

}

BackGround::~BackGround(){
    if (scene) delete scene;
    if (img) delete img;
}

//Add a shape!
void BackGround::addPolygon(QPolygon polygon, QColor color){
    if (!scene){
        scene=new QGraphicsScene(polygon.boundingRect());
        scene->setBackgroundBrush(Qt::transparent);
        scene->setItemIndexMethod(QGraphicsScene::NoIndex);
        scene->setSortCacheEnabled(true);

    }
    else {
        QRectF r=scene->sceneRect();
        r=r.unite(polygon.boundingRect());
        r.setWidth(r.width()+40);
        r.setHeight(r.height()+40);
        r.setLeft(r.left()-20);
        r.setTop(r.top()-20);
        scene->setSceneRect(r);
    }
    polygons.push_back(QPair<QPolygon,QColor>(polygon,color));
    QGraphicsPolygonItem * item=new QGraphicsPolygonItem(polygon);
    item->setBrush(color);
    item->setPen(QPen(Qt::transparent,0));
    item->setActive(true);

    QGraphicsBlurEffect *effect=new QGraphicsBlurEffect();
    effect->setBlurRadius(20*1/this->id);
    effect->setBlurHints(QGraphicsBlurEffect::QualityHint);
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
        if (img==NULL){
            img=new QImage(scene->width(),scene->height(),QImage::Format_ARGB32_Premultiplied);
            QPainter * pimage=new QPainter(img);
            scene->render(pimage,QRect(0,0,scene->width(),scene->height()));

        }
        p.drawImage(scene->sceneRect(),*img);
//        QGraphicsItem *item;
//        QStyleOptionGraphicsItem option;
//        option.state=QStyle::State_Enabled;

//        for (int i=0;i<scene->items().length();i++){
//            Q_ASSERT(item);

//            item=scene->items()[i];

//            p.save();
//            p.setMatrix(scene->items()[i]->sceneMatrix(),true);

//            //            p.setWorldTransform(item->graphicsEffect()->);
//            scene->items()[i]->paint(&p,&option);
//            p.restore();
//        }
        p.restore();

}
