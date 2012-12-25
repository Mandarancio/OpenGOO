#include "og_levelconfig.h"

OGLevelConfig::OGLevelConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("level");
}

void OGLevelConfig::Parser(OGLevel & level)
{
    for(QDomNode n = rootElement.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        QDomElement domElement = n.toElement();

        if (domElement.tagName() == "camera")
        {
            OGCamera camera;

            camera.aspect = domElement.attribute("aspect", "");

            QDomNode node = domElement.firstChild();

            QDomElement element = node.toElement();

            if (element.tagName() == "poi")
            {
                QStringList posList(element.attribute("pos", "").split(","));
                camera.pos.setX(posList.at(0).toDouble());
                camera.pos.setY(posList.at(1).toDouble());
            }

            level.cameras << camera;
        }
        else if (domElement.tagName() == "music")
        {
            level.music = domElement.attribute("music", "");
        }
    }
}


