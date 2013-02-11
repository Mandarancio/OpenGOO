#include "og_sceneconfig.h"

#include <QList>
#include <QDebug>


OGSceneConfig::OGSceneConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("scene");
}

WOGScene* OGSceneConfig::Parser()
{
    WOGScene* scene = new WOGScene;
    scene->minx = rootElement.attribute("minx").toDouble();
    scene->miny = rootElement.attribute("miny").toDouble();
    scene->maxx = rootElement.attribute("maxx").toDouble();
    scene->maxy = rootElement.attribute("maxy").toDouble();
    scene->backgroundcolor =
            StringToColor(rootElement.attribute("backgroundcolor"));

    QDomNode node =rootElement.firstChild();

    while (!node.isNull())
    {
        QDomElement domElement = node.toElement();

        if (domElement.tagName() == "SceneLayer")
        {            
            scene->sceneLayer << CreateSceneLayer(domElement);
        }
        else if (domElement.tagName() == "label")
        {            
            scene->label << CreateLabel(domElement);
        }
        else if (domElement.tagName() == "buttongroup")
        {            
            scene->buttongroup << CreateButtonGroup(domElement);
        }
        else if (domElement.tagName() == "button")
        {
            scene->button << CreateButton(domElement);
        }
        else if (domElement.tagName() == "radialforcefield")
        {
            scene->radialforcefield << CreateRadialForceField(domElement);
        }
        else if (domElement.tagName() == "linearforcefield")
        {
            scene->linearforcefield << CreateLinearForceField(domElement);
        }
        else if (domElement.tagName() == "particles")
        {
            scene->particle << CreateParticle(domElement);
        }
        else if (domElement.tagName() == "circle")
        {
            scene->circle << CreateCircle(domElement);
        }
        else if (domElement.tagName() == "line")
        {
            scene->line << CreateLine(domElement);
        }
        else if (domElement.tagName() == "rectangle")
        {
            scene->rectangle << CreateRectanle(domElement);
        }

        node = node.nextSibling();
    }

    return scene;
}

WOGButton* OGSceneConfig::CreateButton(const QDomElement & element)
{
    WOGButton* obj = new WOGButton;
    obj->id = element.attribute("id");
    obj->depth = element.attribute("depth").toDouble();
    obj->position = StringToPoint(element.attribute("x")
                                  , element.attribute("y")
                                  );

    obj->scale = StringToPoint(element.attribute("scalex")
                               , element.attribute("scaley")
                               );

    obj->rotation = element.attribute("rotation").toDouble();
    obj->alpha = element.attribute("alpha", "1").toDouble();
    obj->colorize = StringToColor(element.attribute("colorize", "255,255,255")
                                  );

    obj->up = element.attribute("up");
    obj->over = element.attribute("over");
    obj->disabled = element.attribute("disabled");
    obj->onclick = element.attribute("onclick");
    obj->onmouseenter = element.attribute("onmouseenter");
    obj->onmouseexit = element.attribute("onmouseexit");

    return obj;
}

WOGLabel* OGSceneConfig::CreateLabel(const QDomElement & element)
{
    WOGLabel* obj = new WOGLabel;
    obj->id = element.attribute("id");
    obj->depth = element.attribute("depth").toDouble();
    obj->position = StringToPoint(element.attribute("x")
                                  , element.attribute("y")
                                  );

    obj->align = element.attribute("align");
    obj->rotation = element.attribute("rotation").toDouble();
    obj->scale = element.attribute("scale").toDouble();
    obj->overlay = StringToBool(element.attribute("overlay"));
    obj->screenspace = StringToBool(element.attribute("screenspace"));
    obj->font = element.attribute("font");
    obj->text = element.attribute("text");

    return obj;
}

WOGSceneLayer* OGSceneConfig::CreateSceneLayer(const QDomElement & element)
{
    WOGSceneLayer* obj = new WOGSceneLayer;
    obj->id = element.attribute("id");
    obj->name = element.attribute("name");
    obj->depth = element.attribute("depth").toDouble();
    obj->position = StringToPoint(element.attribute("x")
                                  , element.attribute("y")
                                  );

    obj->scale = StringToPoint(element.attribute("scalex")
                               , element.attribute("scaley")
                               );

    obj->rotation = element.attribute("rotation").toDouble();
    obj->alpha = element.attribute("alpha", "1").toDouble();
    obj->colorize = StringToColor(element.attribute("colorize", "255,255,255")
                                  );

    obj->image = element.attribute("image");
    obj->anim = element.attribute("anim");
    obj->animspeed = element.attribute("animspeed").toDouble();

    return obj;
}

WOGCircle* OGSceneConfig::CreateCircle(const QDomElement & element)
{
    WOGCircle* obj = new WOGCircle;
    obj->position = StringToPoint(element.attribute("x")
                                  , element.attribute("y")
                                  );

    obj->radius = element.attribute("radius").toDouble();

    CreatePObject(obj, element);

    return obj;
}

WOGLinearForceField* OGSceneConfig::CreateLinearForceField(const QDomElement & element)
{
    WOGLinearForceField* obj = new WOGLinearForceField;
    obj->type = element.attribute("type");
    obj->force = StringToPoint(element.attribute("force"));
    obj->dampeningfactor = element.attribute("dampeningfactor").toDouble();
    obj->antigrav = StringToBool(element.attribute("antigrav"));
    obj->geomonly = StringToBool(element.attribute("geomonly"));

    return obj;
}

WOGParticle* OGSceneConfig::CreateParticle(const QDomElement & element)
{
    WOGParticle* obj = new WOGParticle;
    obj->effect = element.attribute("effect");
    obj->depth = element.attribute("depth").toDouble();
    obj->position = StringToPoint(element.attribute("pos"));
    obj->pretick = element.attribute("pretick").toDouble();

    return obj;
}

WOGButtonGroup* OGSceneConfig::CreateButtonGroup(const QDomElement & element)
{
    WOGButtonGroup* obj = new WOGButtonGroup;
    obj->id = element.attribute("id");
    obj->osx = StringToPoint(element.attribute("osx"));

    for(QDomNode n=element.firstChild(); !n.isNull(); n=n.nextSibling())
    {
        obj->button << CreateButton(n.toElement());
    }

    return obj;
}

WOGLine* OGSceneConfig::CreateLine(const QDomElement & element)
{
    WOGLine* obj = new WOGLine;
    obj->anchor = StringToPoint(element.attribute("anchor"));
    obj->normal = StringToPoint(element.attribute("normal"));

    CreatePObject(obj, element);

    return obj;
}

WOGRadialForceField* OGSceneConfig::CreateRadialForceField(const QDomElement & element)
{
    WOGRadialForceField* obj = new WOGRadialForceField;
    obj->id = element.attribute("id");
    obj->type = element.attribute("type");
    obj->center = StringToPoint(element.attribute("center"));
    obj->radius = element.attribute("radius").toDouble();
    obj->forceatcenter = element.attribute("forceatcenter").toDouble();
    obj->forceatedge = element.attribute("forceatedge").toDouble();
    obj->dampeningfactor = element.attribute("dampeningfactor").toDouble();
    obj->antigrav = StringToBool(element.attribute("antigrav"));
    obj->geomonly = StringToBool(element.attribute("geomonly"));
    obj->enabled = StringToBool(element.attribute("enabled"));

    return obj;
}

WOGRectangle* OGSceneConfig::CreateRectanle(const QDomElement & element)
{
    WOGRectangle*  obj = new WOGRectangle;
    obj->position = StringToPoint(element.attribute("x")
                                  , element.attribute("y")
                                  );

    obj->size = StringToSize(element.attribute("width")
                             , element.attribute("height")
                             );

    obj->rotation =element.attribute("rotation").toDouble();

    CreatePObject(obj, element);

    return obj;
}

void OGSceneConfig::CreatePObject(WOGPObject* pobject
                                  , const QDomElement & element)
{
    pobject->id = element.attribute("id");

    if (StringToBool(element.attribute("static")))
    {
        pobject->dynamic = false;
    }
    else { pobject->dynamic = true; }

    pobject->tag = element.attribute("tag", "walkable");
    pobject->material = element.attribute("material");
}
