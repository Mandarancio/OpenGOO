#include "og_pipe.h"
#include "opengoo.h"
#include "og_world.h"
#include "og_sprite.h"

#include <QPainter>
#include <QPointer>

#include <memory>

OGPipe::OGPipe(WOGPipe* pipe)
{
    QString type;

    if (pipe->type == "ISH") type = "_ISH";
    else if (pipe->type == "BEAUTY") type = "_BEAUTY";
    else if (pipe->type == "BLACK") type = "_BLACK";
    else type = "_NORMAL";

    std::auto_ptr<QImage> pipeH;

    pipeH.reset(_GetImage("IMAGE_GLOBAL_PIPE" + type));

    QTransform transform;
    transform.rotate(90);
    QImage pipeV = pipeH->transformed(transform);

    QPixmap pixmapH = QPixmap::fromImage(*pipeH);
    QPixmap pixmapV = QPixmap::fromImage(pipeV);

    QLineF line;
    QPointF p1;
    QPointF p2;
    float length;
    float angle;

    const float pipeWidth = 36;

    QList<QPointF>* verteces = pipe->vertex;

    for (int i = 0; i < verteces->size() - 1; i++)
    {
        p1 = verteces->at(i);
        p2 = verteces->at(i + 1);

        line = QLineF(p1, p2);
        length = line.length();
        angle = line.angle();

        if (angle == 0 || angle == 180)
        {
            OGSprite* sprite = new OGSprite;
            sprite->image = pixmapH;
            sprite->depth = pipe->depth;
            sprite->visible = true;
            sprite->size.setWidth(length);
            sprite->size.setHeight(pipeWidth);

            if (angle == 0) sprite->position.setX(p1.x());
            else sprite->position.setX(p1.x() - length);

            sprite->position.setY((p1.y() + pipeWidth * 0.5f) * -1);

            _InsertSprite(sprite);
        }
        else if (angle == 90 || angle == 270)
        {
            OGSprite* sprite = new OGSprite;
            sprite->image = pixmapV;
            sprite->depth = pipe->depth;

            sprite->visible = true;

            sprite->size.setWidth(pipeWidth);
            sprite->size.setHeight(length);

            sprite->position.setX(p1.x() - pipeWidth * 0.5f);

            if (angle == 270) sprite->position.setY((p1.y() + length) * -1);
            else sprite->position.setY(p1.y() * -1);

            _InsertSprite(sprite);
        }
    }
}

QImage* OGPipe::_GetImage(const QString &id)
{
    WOGResources* rm = _GetResourceManager();

    return new QImage(rm->GetImage(id) + ".png");
}

WOGResources* OGPipe::_GetResourceManager()
{
    OGWorld* world = OpenGOO::instance()->GetWorld();
    return world->resrcdata();
}

void OGPipe::_InsertSprite(OGSprite *sprite)
{
    OGWorld* world = OpenGOO::instance()->GetWorld();
    world->sprites_ << sprite;
}
