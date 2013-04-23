#include "og_pipe.h"
#include "opengoo.h"
#include "og_world.h"
#include "og_sprite.h"
#include "wog_pipe.h"
#include "wog_resources.h"

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
    QPointF p3;
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

    if (verteces->size() > 2)
    {
        OGSprite* bend;

        for (int i = 0; i < verteces->size() - 2; i++)
        {
            p1 = verteces->at(i);
            p2 = verteces->at(i + 1);
            p3 = verteces->at(i + 2);

            bend = _CreateBend(type, p1, p2, p3, pipe->depth);

            if (bend) _InsertSprite(bend);
        }
    }

    pCapClosed_ = _CreateCap(pipe, "IMAGE_GLOBAL_PIPE_CAP_CLOSED" + type
                             , true);

    if (pCapClosed_) _InsertSprite(pCapClosed_);

    pCapOpen_ = _CreateCap(pipe, "IMAGE_GLOBAL_PIPE_CAP_OPEN" + type
                           , false);

    if (pCapOpen_) _InsertSprite(pCapOpen_);
}

void OGPipe::_Close()
{
    pCapOpen_->visible = false;
    pCapClosed_->visible = true;
}

void OGPipe::_Open()
{
    pCapOpen_->visible = true;
    pCapClosed_->visible = false;
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

void OGPipe::_InsertSprite(OGSprite* sprite)
{
    OGWorld* world = OpenGOO::instance()->GetWorld();
    world->_InsertSprite(sprite);
}

OGSprite* OGPipe::_CreateCap(WOGPipe* pipe, const QString &id,  bool visible)
{
    std::auto_ptr<QImage> src;
    src.reset(_GetImage(id));

    if (src.get() == 0 || pipe->vertex->size() < 2) return 0;

    QPointF p1 = pipe->vertex->at(0);
    QPointF p2 = pipe->vertex->at(1);

    float angle = QLineF(p2, p1).angle() - 90;

    const float w = 80;
    const float h = 80;

    float x = p1.x() - w * 0.5f;
    float y = (p1.y() + h * 0.5f) * -1;

    OGSprite* sprite = new OGSprite;

    if (angle != 0)
    {
        QTransform transform;
        transform.rotate(angle);
        sprite->image = QPixmap::fromImage(src->transformed(transform));
    }
    else
    {
        sprite->image = QPixmap::fromImage(*src);
    }

    sprite->position = QPointF(x, y);
    sprite->size = QSize(w, h);
    sprite->visible = visible;
    sprite->depth = pipe->depth;

    return sprite;
}

OGSprite* OGPipe::_CreateBend(const QString &type, const QPointF &p1, const QPointF &p2
                              , const QPointF &p3, float depth)
{
    OGSprite* sprite = 0;
    float angle1 = QLineF(p1, p2).angle();
    float angle2 = QLineF(p2, p3).angle();
    float w = 54;
    float h = 54;
    float x = p2.x() - w / 2;
    float y = (p2.y() + h / 2) * -1;
    std::auto_ptr<QImage> img;

    if ((angle1 == 0 && angle2 == 270) || (angle1 == 90 && angle2 == 180))
    {
        img.reset(_GetImage("IMAGE_GLOBAL_PIPE_BEND_BR" + type));
    }
    else if ((angle1 == 90 && angle2 == 0) || (angle1 == 180 && angle2 == 270))
    {
        img.reset(_GetImage("IMAGE_GLOBAL_PIPE_BEND_BL" + type));
    }
    else if ((angle1 == 270 && angle2 == 180) || (angle1 == 0 && angle2 == 90))
    {
        img.reset(_GetImage("IMAGE_GLOBAL_PIPE_BEND_TR" + type));
    }
    else if ((angle1 == 180 && angle2 == 90) || (angle1 == 270 && angle2 == 0))
    {
        img.reset(_GetImage("IMAGE_GLOBAL_PIPE_BEND_TL" + type));
    }

    if (img.get() != 0)
    {
        sprite = new OGSprite;
        sprite->image = QPixmap::fromImage(*img);
        sprite->position = QPoint(x, y);
        sprite->size = QSize(w, h);
        sprite->visible = true;
        sprite->depth = depth;
    }

    return sprite;
}
