#include "og_ballconfig.h"

#include <QDebug>

OGBallConfig::OGBallConfig(const QString &filename)
    : OGXmlConfig(filename)
{
    SetRootTag("ball");
}

WOGBall* OGBallConfig::Parser()
{
    WOGBall* obj = new WOGBall;

    CreateAttributes_(obj);

    return obj;
}

void OGBallConfig::CreateAttributes_(WOGBall* ball)
{
    CreateCoreAttributes_(ball);
    CreateBehaviourAttributes_(ball);
    CreateMovementAttributes_(ball);
    CreatePlayerInteraction_(ball);
    CreateCosmeticAttributes_(ball);

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
    ball->attribute.core.shape = rootElement.attribute("shape");
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
}

void OGBallConfig::CreateCosmeticAttributes_(WOGBall* ball)
{
    ball->attribute.cosmetic.blinkcolor =
            StringToColor(rootElement.attribute("blinkcolor"));
}
