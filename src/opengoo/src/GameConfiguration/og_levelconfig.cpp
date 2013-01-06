#include "og_levelconfig.h"

OGLevelConfig::OGLevelConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("level");
}

OGLevel OGLevelConfig::Parser()
{
    OGLevel level;

    level.ballsrequired = rootElement.attribute("ballsrequired").toInt();    
    level.letterboxed = StringToBool(rootElement.attribute("letterboxed"));
    level.visualdebug = StringToBool(rootElement.attribute("visualdebug"));
    level.autobounds = StringToBool(rootElement.attribute("autobounds"));
    level.textcolor = StringToColor(rootElement.attribute("textcolor"));
    level.texteffects = StringToBool(rootElement.attribute("texteffects"));
    level.timebugprobability =
            rootElement.attribute("timebugprobability").toDouble();

    level.strandgeom = StringToBool(rootElement.attribute("strandgeom"));
    level.allowskip = StringToBool(rootElement.attribute("allowskip"));

    for(QDomNode n=rootElement.firstChild(); !n.isNull(); n=n.nextSibling())
    {
        QDomElement domElement = n.toElement();

        if (domElement.tagName() == "camera")
        {
            OGCamera camera;

            camera.aspect = domElement.attribute("aspect");            
            camera.endpos = StringToPoint(domElement.attribute("endpos"));
            camera.endzoom = domElement.attribute("endzoom").toDouble();

            QDomNode node = domElement.firstChild();

            for (; !node.isNull(); node=node.nextSibling())
            {
                QDomElement element = node.toElement();

                if (element.tagName() == "poi")
                {
                    OGPoi poi = {
                        StringToPoint(element.attribute("pos")),
                        element.attribute("traveltime").toDouble(),
                        element.attribute("pause").toDouble(),
                        element.attribute("zoom").toDouble()
                    };

                    camera.poi << poi;
                }
            }

            level.camera << camera;
        }
        else if (domElement.tagName() == "music")
        {
            level.music.id = domElement.attribute("music");
        }
        else if (domElement.tagName() == "BallInstance")
        {
            OGBallInstance ball;

            ball.type = domElement.attribute("type");
            ball.position =
                    StringToPoint(
                        domElement.attribute("x"),
                        domElement.attribute("y")
                        );

            ball.id = domElement.attribute("id");
            ball.angle = domElement.attribute("angle").toDouble();
            ball.discovered = StringToBool(domElement.attribute("discovered"));

            level.ball << ball;
        }
    }

    return level;
}
