#include "og_levelconfig.h"
#include "wog_level.h"

struct OGLevelConfigHelper
{
    static WOGCamera CreateCamera(const QDomElement& element);
    static WOGPoi CreatePoi(const QDomElement& element);
    static WOGBallInstance CreateBallInstance(const QDomElement& element);
    static WOGStrand CreateStrand(const QDomElement& element);
    static QPointF CreateVertex(const QDomElement& element);

    static void LoadLevelExit(WOGLevelExit& obj, const QDomElement& element);
    static void LoadPipe(WOGPipe& pipe, const QDomElement& element);
};

WOGPoi OGLevelConfigHelper::CreatePoi(const QDomElement &element)
{
    WOGPoi obj;
    obj.position = OGXmlConfig::StringToPointF(element.attribute("pos"));
    obj.traveltime = element.attribute("traveltime").toDouble();
    obj.pause = element.attribute("pause").toDouble();
    obj.zoom = element.attribute("zoom").toDouble();

    return obj;
}

WOGCamera OGLevelConfigHelper::CreateCamera(const QDomElement& element)
{
    WOGCamera obj;
    auto aspect = element.attribute("aspect", "normal");
    if (aspect == "normal")
    {
        obj.aspect = WOGCamera::Normal;
    }
    else if (aspect == "widescreen")
    {
        obj.aspect = WOGCamera::WideScreen;
    }
    else
    {
        obj.aspect = WOGCamera::Unknown;
    }

    obj.endpos = OGXmlConfig::StringToPointF(element.attribute("endpos"));
    obj.endzoom = element.attribute("endzoom", "1").toDouble();

    for (auto node = element.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        obj.poi.push_back(CreatePoi(node.toElement()));
    }

    return obj;
}

WOGBallInstance OGLevelConfigHelper::CreateBallInstance(const QDomElement& element)
{
    WOGBallInstance obj;
    obj.type = element.attribute("type");
    obj.x = element.attribute("x").toFloat();
    obj.y = element.attribute("y").toFloat();

    obj.id = element.attribute("id");
    obj.angle = element.attribute("angle").toFloat();
    obj.discovered = OGXmlConfig::StringToBool(element.attribute("discovered", "true"));

    return obj;
}

QPointF OGLevelConfigHelper::CreateVertex(const QDomElement &element)
{
    float x = element.attribute("x").toFloat();
    float y = element.attribute("y").toFloat();

    return QPointF(x, y);
}

WOGStrand OGLevelConfigHelper::CreateStrand(const QDomElement& element)
{
    WOGStrand obj;
    obj.gb1 = element.attribute("gb1");
    obj.gb2 = element.attribute("gb2");

    return obj;
}

void OGLevelConfigHelper::LoadLevelExit(WOGLevelExit& obj, const QDomElement &element)
{
    obj.id = element.attribute("id");
    obj.pos = OGXmlConfig::StringToPointF(element.attribute("pos"));
    obj.radius = element.attribute("radius").toFloat();
    obj.filter = element.attribute("filter");
}

void OGLevelConfigHelper::LoadPipe(WOGPipe& pipe, const QDomElement &element)
{
    pipe.id = element.attribute("id", "0");
    pipe.depth = element.attribute("depth", "0").toFloat();
    pipe.type = element.attribute("type");
    for (auto node = element.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        pipe.vertex.push_back(CreateVertex(node.toElement()));
    }
}

OGLevelConfig::Type OGLevelConfig::Parser()
{
    auto level = Type(new WOGLevel);
    level->levelexit.second = false;
    level->pipe.second = false;

    level->ballsrequired = rootElement.attribute("ballsrequired").toInt();
    level->letterboxed = StringToBool(rootElement.attribute("letterboxed"));
    level->visualdebug = StringToBool(rootElement.attribute("visualdebug"));
    level->autobounds = StringToBool(rootElement.attribute("autobounds"));
    level->textcolor = StringToColor(rootElement.attribute("textcolor"));
    level->texteffects = StringToBool(rootElement.attribute("texteffects"));
    level->timebugprobability = rootElement.attribute("timebugprobability").toDouble();

    level->strandgeom = StringToBool(rootElement.attribute("strandgeom"));
    level->allowskip = StringToBool(rootElement.attribute("allowskip"));

    for (auto node = rootElement.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        auto domElement = node.toElement();

        if (domElement.tagName() == "camera")
        {
            level->camera.push_back(OGLevelConfigHelper::CreateCamera(domElement));
        }
        else if (domElement.tagName() == "music")
        {
            level->music.id = domElement.attribute("id");
        }
        else if (domElement.tagName() == "BallInstance")
        {
            level->ball.push_back(OGLevelConfigHelper::CreateBallInstance(domElement));
        }
        else if (domElement.tagName() == "levelexit")
        {
            if (!level->levelexit.second)
            {
                OGLevelConfigHelper::LoadLevelExit(level->levelexit.first, domElement);
                level->levelexit.second = true;
            }
        }
        else if (domElement.tagName() == "pipe")
        {
            if (!level->pipe.second)
            {                
                OGLevelConfigHelper::LoadPipe(level->pipe.first, domElement);
                level->pipe.second = true;
            }
        }
        else if (domElement.tagName() == "Strand")
        {
            level->strand.push_back(OGLevelConfigHelper::CreateStrand(domElement));
        }
    }

    return level;
}
