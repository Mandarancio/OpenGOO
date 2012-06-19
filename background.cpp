#include "background.h"
#include "flags.h"

#include <QGraphicsBlurEffect>
#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QImage>
#include <QPainter>


class BackGround::ImageCache
{
    bool img_valid; //!< false if #img needs to be updated
    QGraphicsScene scene;
    QImage img;

public:
    ImageCache();
    
    /*! add polygon to scene and #img_valid = false */
    void addPolygon(const QPolygon& polygon, const QColor& color, int level);
    
    /*! if(!#img_valid) {update #img; #img_valid = true;} draw #img to QPainter. */
    void paint(QPainter& p);
};


BackGround::ImageCache::ImageCache() :
    img_valid(false),
    scene(),
    img()
{
    scene.setBackgroundBrush(Qt::transparent);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);
    scene.setSortCacheEnabled(true);
}

void BackGround::ImageCache::addPolygon(const QPolygon& polygon, const QColor& color, int level)
{
    scene.setSceneRect(
        scene.sceneRect()
        .united(polygon.boundingRect())
        .adjusted(-20, -20, 20, 20)
    );
    
    QGraphicsPolygonItem *item = new QGraphicsPolygonItem(polygon);
    item->setBrush(color);
    item->setPen(QPen(Qt::transparent,0));
    item->setActive(true);
    
    QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
    effect->setBlurRadius(20.0 / qreal(level));
    effect->setBlurHints(QGraphicsBlurEffect::QualityHint);
    item->setGraphicsEffect(effect); // takes ownership
    
    item->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    
    scene.addItem(item); // takes ownership
    img_valid = false;
}

void BackGround::ImageCache::paint(QPainter& p)
{
    if (!img_valid) {
        img = QImage(scene.width(), scene.height(), QImage::Format_ARGB32_Premultiplied);
        img.fill(QColor(0,0,0,0).rgba()); // required: qpainter cannot draw on null image
        QPainter pimage(&img);
        scene.render(&pimage);
        img_valid = true;
    }
    p.drawImage(scene.sceneRect(), img);
}



BackGround::BackGround(int level, QObject* parent) :
    QObject(parent),
    level(level),
    delta(0),
    translate(QPoint(0,0)),
    img_cache(flag & OPENGL ? NULL : new ImageCache()),
    polygons()
{
}

BackGround::~BackGround() {} // required to use QScopedPointer on forward-declarated type

void BackGround::addPolygon(const QPolygon& polygon, const QColor& color)
{
    if (img_cache)
        img_cache->addPolygon(polygon, color, level);
    else
        polygons.push_back(QPair<QPolygon, QColor>(polygon, color));
}

void BackGround::paint(QPainter& p)
{
    p.save();
    p.translate(translate);
    p.setPen(Qt::transparent);
    
    if (img_cache) {
        img_cache->paint(p);
    }
    else {
        typedef QPair<QPolygon, QColor> pair_type; // it's required, because foreach cannot use type that contains commas. (preprocessor issue)
        foreach (const pair_type& pair, polygons) {
            p.setBrush(pair.second);
            p.drawPolygon(pair.first);
        }
    }
    p.restore();
}
