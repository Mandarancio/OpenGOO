#include "og_levelconfig.h"

OGLevelConfig::OGLevelConfig(const QString &filename)
    : OGXmlConfig(filename)
{
    SetRootTag("level");
}

WOGLevel* OGLevelConfig::Parser()
{
    WOGLevel* level = new WOGLevel;

    level->ballsrequired = rootElement.attribute("ballsrequired").toInt();
    level->letterboxed = StringToBool(rootElement.attribute("letterboxed"));
    level->visualdebug = StringToBool(rootElement.attribute("visualdebug"));
    level->autobounds = StringToBool(rootElement.attribute("autobounds"));
    level->textcolor = StringToColor(rootElement.attribute("textcolor"));
    level->texteffects = StringToBool(rootElement.attribute("texteffects"));
    level->timebugprobability =
        rootElement.attribute("timebugprobability").toDouble();

    level->strandgeom = StringToBool(rootElement.attribute("strandgeom"));
    level->allowskip = StringToBool(rootElement.attribute("allowskip"));

    QDomNode node = rootElement.firstChild();

    while (!node.isNull())
    {
        QDomElement domElement = node.toElement();

        if (domElement.tagName() == "camera")
        {
            level->camera << CreateCamera(domElement);
        }
        else if (domElement.tagName() == "music")
        {
            level->music.id = domElement.attribute("music");
        }
        else if (domElement.tagName() == "BallInstance")
        {
            level->ball << CreateBallInstance(domElement);
        }
        else if (domElement.tagName() == "levelexit")
        {
            if (!level->levelexit)
            {
                level->levelexit = CreateLevelExit(domElement);
            }
        }
        else if (domElement.tagName() == "pipe")
        {
            if (!level->pipe)
            {
                level->pipe = CreatePipe(domElement);
            }
        }
        else if (domElement.tagName() == "Strand")
        {
            level->strand << CreateStrand(domElement);
        }

        node = node.nextSibling();
    }

    return level;
}

WOGLevelExit* OGLevelConfig::CreateLevelExit(const QDomElement &element)
{
    WOGLevelExit* obj = new WOGLevelExit;
    obj->id = element.attribute("id");
    obj->pos = StringToPoint(element.attribute("pos"));
    obj->radius = element.attribute("radius").toFloat();
    obj->filter = element.attribute("filter");

    return obj;
}

WOGPoi* OGLevelConfig::CreatePoi(const QDomElement &element)
{
    WOGPoi* obj = new WOGPoi;
    obj->position = StringToPoint(element.attribute("pos"));
    obj->traveltime = element.attribute("traveltime").toDouble();
    obj->pause = element.attribute("pause").toDouble();
    obj->zoom = element.attribute("zoom").toDouble();

    return obj;
}

WOGCamera* OGLevelConfig::CreateCamera(const QDomElement &element)
{
    WOGCamera* obj = new WOGCamera;

    obj->aspect = element.attribute("aspect");
    obj->endpos = StringToPoint(element.attribute("endpos"));
    obj->endzoom = element.attribute("endzoom").toDouble();

    QDomNode node = element.firstChild();

    while (!node.isNull())
    {
        obj->poi << CreatePoi(node.toElement());
        node = node.nextSibling();
    }

    return obj;
}

WOGBallInstance* OGLevelConfig::CreateBallInstance(const QDomElement &element)
{
    WOGBallInstance* obj = new WOGBallInstance;
    obj->type = element.attribute("type");
    obj->x = element.attribute("x").toFloat();
    obj->y = element.attribute("y").toFloat();

    obj->id = element.attribute("id");
    obj->angle = element.attribute("angle").toFloat();
    obj->discovered = StringToBool(element.attribute("discovered", "true"));

    return obj;
}

QPointF OGLevelConfig::CreateVertex(const QDomElement &element)
{

    float x = element.attribute("x").toFloat();
    float y = element.attribute("y").toFloat();

    return QPointF(x, y);
}

WOGPipe* OGLevelConfig::CreatePipe(const QDomElement &element)
{
    WOGPipe* pipe = new WOGPipe;
    pipe->id = element.attribute("id");
    pipe->depth = element.attribute("depth").toFloat();
    pipe->type = element.attribute("type");
    pipe->vertex = new QList<QPointF>;
    QDomNode node = element.firstChild();

    while (!node.isNull())
    {
        pipe->vertex->append(CreateVertex(node.toElement()));
        node = node.nextSibling();
    }

    return pipe;
}

WOGStrand* OGLevelConfig::CreateStrand(const QDomElement &element)
{
    WOGStrand* obj = new WOGStrand;
    obj->gb1 = element.attribute("gb1");
    obj->gb2 = element.attribute("gb2");

    return obj;
}
