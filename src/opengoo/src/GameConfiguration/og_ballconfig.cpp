#include "og_ballconfig.h"

//template<typename TTagParser>
//inline void OGBallConfig<TTagParser>::CreateBehaviourAttributes_(WOGBall* ball)
//{
//    ball->attribute.behaviour.jump = rootElement.attribute("jump", "0");
//}

//template<typename TTagParser>
//inline void OGBallConfig<TTagParser>::CreateMovementAttributes_(WOGBall* ball)
//{
//    ball->attribute.movement.climbspeed = rootElement.attribute("climbspeed").toFloat();
//    ball->attribute.movement.speedvariance = rootElement.attribute("speedvariance").toFloat();
//    ball->attribute.movement.walkforce = rootElement.attribute("walkforce").toFloat();
//    ball->attribute.movement.walkspeed = rootElement.attribute("walkspeed").toFloat();
//}

//template<typename TTagParser>
//inline void OGBallConfig<TTagParser>::CreatePlayerInteraction_(WOGBall* ball)
//{
//    ball->attribute.player.detachable = StringToBool(rootElement.attribute("detachable", "true"));
//    ball->attribute.player.draggable = StringToBool(rootElement.attribute("draggable", "true"));
//    ball->attribute.player.hingedrag = StringToBool(rootElement.attribute("hingedrag", "false"));
//    ball->attribute.player.fling = rootElement.attribute("fling");
//}

//template<typename TTagParser>
//inline void OGBallConfig<TTagParser>::CreateCosmeticAttributes_(WOGBall* ball)
//{
//    ball->attribute.cosmetic.blinkcolor = StringToColor(rootElement.attribute("blinkcolor"));
//}

//template<typename TTagParser>
//inline void OGBallConfig<TTagParser>::_CreateLevelInteraction(WOGBall* ball)
//{
//    ball->attribute.level.suckable = StringToBool(rootElement.attribute("suckable", "true"));
//}

//template<typename TTagParser>
//inline void OGBallConfig<TTagParser>::ReadAttributes(TTagParser::Type *ball)
//{
//    CreateBehaviourAttributes_(ball);
//    CreateMovementAttributes_(ball);
//    CreatePlayerInteraction_(ball);
//    CreateCosmeticAttributes_(ball);
//    _CreateLevelInteraction(ball);

//    ball->attribute.spawn = rootElement.attribute("spawn", "");
//}
