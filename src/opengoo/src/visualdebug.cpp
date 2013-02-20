#include "og_world.h"
#include "flags.h"

#include <QPainter>
#include <QTime>

extern bool _E404;
extern int _fps;
extern OGBall* _nearestBall;
extern OGButton _buttonMenu;
extern QList<OGStrand*> _strands;
extern QList<OGBall*> _balls;
extern OGBall* _selectedBall;

namespace visual_debug
{
    const qreal K = 10.0;
    bool _initImages = true;
    QList<QImage> _images;
    const qreal DEGREE = 57.2957795;
}

using namespace visual_debug;

void showFPS(QPainter* painter, qreal zoom)
{
    int x, y;

    x = painter->window().x() + 20.0/zoom;
    y = painter->window().y() + 20.0/zoom;
    painter->setPen(Qt::white);
    painter->setFont(QFont("Verdana", qRound(12.0/zoom), QFont::Bold));
    painter->drawText(x, y, QString::number(_fps));
}

void createImage(const QSizeF size, qreal angle)
{
    QTransform transform;
    transform.rotate(angle*-DEGREE);
    QColor color(0, 255, 0, 100);
    QImage image(size.toSize(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(image.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.fillRect(image.rect(), color);
    painter.end();

    _images << image.transformed(transform);
}

void createImages(OGWorld* world)
{
    Q_FOREACH (WOGRectangle* rectangle, world->scenedata()->rectangle)
    {
            createImage(rectangle->size, rectangle->rotation);
    }
}

QPointF getPosTarget(const QPointF & pos, const QSizeF & size)
{
    qreal y = pos.y()*(-1.0);
    QPointF p(pos.x(), y);
    QRectF rect(p, size);
    rect.moveCenter(p);

    return rect.topLeft();
}

void drawRect(QPainter *painter, OGWorld* world)
{
    int i = 0;

    if (_initImages)
    {
        createImages(world);
        _initImages = false;
    }

    QPointF pos;

    Q_FOREACH (WOGRectangle* rectangle, world->scenedata()->rectangle)
    {
        pos = getPosTarget(rectangle->position
                           , _images.at(i).size());

        painter->drawImage(QRectF(pos, _images.at(i).size())
                           , _images.at(i), _images.at(i).rect());
        i++;
    }
}

void visualDebug(QPainter* painter, OGWorld* world, qreal zoom)
{
    qreal winX = painter->window().x();
    qreal winY = painter->window().y();

    QPen pen(Qt::yellow,  2.0/zoom);

    painter->setOpacity(1);
    painter->setPen(pen);

    // menu
    qreal btnW = _buttonMenu.size().width()/zoom;
    qreal btnH = _buttonMenu.size().height()/zoom;
    qreal btnX = winX + _buttonMenu.position().x()/zoom;
    qreal btnY = winY + _buttonMenu.position().y()/zoom;
    QRectF rect(btnX, btnY, btnW, btnH);

    painter->drawRect(rect);
    painter->drawEllipse(QPointF(0, 0), 10.0/zoom, 10.0/zoom); // center of word

    // level exit
    if (world->leveldata()->levelexit)
    {
        qreal x, y;

        x = world->leveldata()->levelexit->pos.x();
        y = world->leveldata()->levelexit->pos.y()*(-1.0);
        QRectF rect(x, y, 10.0 , 10.0);

        rect.moveCenter(rect.topLeft());
        painter->fillRect(rect, Qt::yellow);
    }

    // pipe
    for (int i=0; i < world->leveldata()->pipe.vertex.size()-1; i++)
    {
        qreal x1, x2, y1, y2;

        x1 = world->leveldata()->pipe.vertex.at(i).x();
        y1 = world->leveldata()->pipe.vertex.at(i).y()*(-1.0);
        x2 = world->leveldata()->pipe.vertex.at(i+1).x();
        y2 = world->leveldata()->pipe.vertex.at(i+1).y()*(-1.0);

        painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }

    for (int i=0; i < world->scenedata()->circle.size(); i++)
    {
        qreal x, y, radius;

        x = world->scenedata()->circle.at(i)->position.x();
        y = world->scenedata()->circle.at(i)->position.y()*(-1.0);
        radius = world->scenedata()->circle.at(i)->radius;

        painter->drawEllipse(QPointF(x, y), radius, radius);
    }

    drawRect(painter, world);   

    qreal x, y;
    b2Vec2 v;

    if (world->nearestball() != 0)
    {
        v = world->nearestball()->GetBodyPosition();
        x = v.x*K;
        y = v.y*K*(-1);

        pen.setColor(Qt::green);
        painter->setPen(pen);
        painter->drawEllipse(QPointF(x, y), 10, 10);
    }

    if (_E404)
    {
        painter->setOpacity(0.25);
        painter->fillRect(painter->window(), Qt::black);
        painter->setOpacity(1);
        painter->setPen(Qt::white);
        painter->setFont(QFont("Times", 36, QFont::Bold));
        painter->drawText(painter->window()
                          , Qt::AlignCenter
                          , "UNIMPLEMENTED!!!\nEsc to return"
                          );
    }

    if (flag == FPS)
    {
        showFPS(painter, zoom);
    }
}
