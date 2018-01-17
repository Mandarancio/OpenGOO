#include "og_ballconfig.h"

#include <QDebug>

//struct WOGPart
//{
//    QString name;
//    QString image;
//    QVector2D origin;
//    int layer;
//    QString state;
//    float scale;
//    bool rotate;
//    QString stretch;
//    bool eye;
//    QString pupil;
//    int pupilinset;
//    int xrange;
//    int yrange;
//};

void OGBallConfig::LoadPartAttr(const QString& a_attrName, const QString& a_attrValue, WOGPart& a_part)
{
    if (a_attrName == "name")
    {
        a_part.name = a_attrValue;
    }
    else if (a_attrName == "image")
    {
        a_part.image = a_attrValue;
    }
}

void OGBallConfig::LoadPart(const QDomElement& a_el, WOGPart& a_part)
{
    auto attributes = a_el.attributes();
    for (int i = 0; i < attributes.size(); ++i)
    {
        auto item = attributes.item(i);
        auto attr = item.toAttr();
        LoadPartAttr(attr.name(), attr.value(), a_part);
    }
}

void LoadSounds(WOGBall& a_ball, const QDomElement& a_el)
{
    a_ball.sounds[a_el.attribute("event")] = a_el.attribute("id").split(",");
}

WOGBall* OGBallConfig::Parser()
{
    QDomElement element;

    auto obj = std::unique_ptr<WOGBall>(new WOGBall);

    CreateAttributes_(obj.get());

    QDomNode node = rootElement.firstChild();

    while (!node.isNull())
    {
        element = node.toElement();

        if (element.tagName() == "strand")
        {
            obj->strand = CreateStrand_(element);
        }
        else if (element.tagName() == "detachstrand")
        {
            obj->detachstrand = CreateDetachstrand_(element);
        }
        else if (element.tagName() == "part")
        {
            WOGPart part;
            LoadPart(element, part);
            obj->parts.push_back(part);
        }
        else if (element.tagName() == "marker")
        {
        }
        else if (element.tagName() == "shadow")
        {

        }
        else if (element.tagName() == "particles")
        {

        }
        else if (element.tagName() == "splat")
        {

        }
        else if (element.tagName() == "sound")
        {
            LoadSounds(*obj, element);
        }
        else if (element.tagName() == "sinvariance")
        {

        }

        node = node.nextSibling();
    }

    return obj.release();
}

void OGBallConfig::CreateAttributes_(WOGBall* ball)
{
    CreateCoreAttributes_(ball);
    CreateBehaviourAttributes_(ball);
    CreateMovementAttributes_(ball);
    CreatePlayerInteraction_(ball);
    CreateCosmeticAttributes_(ball);
    _CreateLevelInteraction(ball);

    ball->attribute.spawn = rootElement.attribute("spawn", "");
}

void OGBallConfig::CreateCoreAttributes_(WOGBall* ball)
{
    ball->attribute.core.dragmass =
            rootElement.attribute("dragmass", "0").toFloat();

    ball->attribute.core.mass =
            rootElement.attribute("mass").toFloat();

    ball->attribute.core.material = rootElement.attribute("material", "");
    ball->attribute.core.name = rootElement.attribute("name");
    ball->attribute.core.shape = StringToShape(rootElement.attribute("shape"));
    ball->attribute.core.strands =
            rootElement.attribute("strands").toInt();

    ball->attribute.core.towermass =
            rootElement.attribute("towermass").toFloat();
}

void OGBallConfig::CreateBehaviourAttributes_(WOGBall* ball)
{
    ball->attribute.behaviour.jump = rootElement.attribute("jump", "0");
}

void OGBallConfig::CreateMovementAttributes_(WOGBall* ball)
{
    ball->attribute.movement.climbspeed =
            rootElement.attribute("climbspeed").toFloat();

    ball->attribute.movement.speedvariance =
            rootElement.attribute("speedvariance").toFloat();

    ball->attribute.movement.walkforce =
            rootElement.attribute("walkforce").toFloat();

    ball->attribute.movement.walkspeed =
            rootElement.attribute("walkspeed").toFloat();
}

void OGBallConfig::CreatePlayerInteraction_(WOGBall* ball)
{
    ball->attribute.player.detachable =
            StringToBool(rootElement.attribute("detachable", "true"));

    ball->attribute.player.draggable =
            StringToBool(rootElement.attribute("draggable", "true"));

    ball->attribute.player.hingedrag =
            StringToBool(rootElement.attribute("hingedrag", "false"));

    ball->attribute.player.fling = rootElement.attribute("fling");
}

void OGBallConfig::CreateCosmeticAttributes_(WOGBall* ball)
{
    ball->attribute.cosmetic.blinkcolor =
            StringToColor(rootElement.attribute("blinkcolor"));
}

WOGBallStrand* OGBallConfig::CreateStrand_(const QDomElement & element)
{
    WOGBallStrand* obj = new WOGBallStrand;

    obj->burntimage = element.attribute("burntimage");
    obj->fireparticles = element.attribute("fireparticles");
    obj->image = element.attribute("image");
    obj->inactiveimage = element.attribute("inactiveimage");
    obj->type = element.attribute("type");

    obj->burnspeed = element.attribute("burnspeed", "0").toFloat();
    obj->dampfac = element.attribute("dampfac", "0").toFloat();
    obj->ignitedelay = element.attribute("ignitedelay", "0").toFloat();
    obj->maxforce = element.attribute("maxforce", "0").toFloat();
    obj->maxlen1 = element.attribute("maxlen1", "0").toFloat();
    obj->maxlen2 = element.attribute("maxlen2", "0").toFloat();
    obj->minlen = element.attribute("minlen", "0").toFloat();
    obj->shrinklen = element.attribute("shrinklen", "140").toFloat();
    obj->springconstmax = element.attribute("springconstmax").toFloat();
    obj->springconstmin = element.attribute("springconstmin").toFloat();
    obj->thickness = element.attribute("thickness").toFloat();

    obj->rope = StringToBool(element.attribute("rope"));
    obj->geom = StringToBool(element.attribute("geom"));
    obj->walkable = StringToBool(element.attribute("walkable", "true"));

    return obj;
}

WOGBallDetachstrand* OGBallConfig::CreateDetachstrand_(
        const QDomElement & element)
{
    WOGBallDetachstrand* obj = new WOGBallDetachstrand;

    obj->image = element.attribute("image");
    obj->maxlen = element.attribute("maxlen", "0").toFloat();

    return obj;
}

std::shared_ptr<WOGBallShape> OGBallConfig::StringToShape(const QString & shape)
{
    auto list = shape.split(",");
    if (list.isEmpty())
        return nullptr;

    auto type = list[0];
    if (type == "circle")
    {
        auto diameter = (list.size() > 1) ? list[1].toFloat() : 0.0f;
        auto variation = (list.size() > 2) ? list[2].toFloat() : 0.0f;

        return std::make_shared<WOGCircleBall>(diameter, variation);
    }

    if (type == "rectangle")
    {
        auto width = (list.size() > 1) ? list[1].toFloat() : 0.0f;
        auto heigth = (list.size() > 2) ? list[2].toFloat() : 0.0f;
        auto variation = (list.size() > 3) ? list[3].toFloat() : 0.0f;

        return std::make_shared<WOGRectangleBall>(width, heigth, variation);
    }

    return nullptr;
}

void OGBallConfig::_CreateLevelInteraction(WOGBall* ball)
{
    ball->attribute.level.suckable =
            StringToBool(rootElement.attribute("suckable", "true"));
}
