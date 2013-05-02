#ifndef OG_BALLCONFIG_H
#define OG_BALLCONFIG_H

#include "og_xmlconfig.h"
#include "wog_ball.h"


class OGBallConfig : public OGXmlConfig
{
public:
    OGBallConfig(const QString & filename);

    WOGBall* Parser();

private:
    void CreateAttributes_(WOGBall* ball);
    void CreateCoreAttributes_(WOGBall* ball);
    void CreateBehaviourAttributes_(WOGBall* ball);
    void CreateMovementAttributes_(WOGBall* ball);
    void CreatePlayerInteraction_(WOGBall* ball);
    void CreateCosmeticAttributes_(WOGBall* ball);
    void _CreateLevelInteraction(WOGBall* ball);
    WOGBallStrand* CreateStrand_(const QDomElement & element);
    WOGBallDetachstrand* CreateDetachstrand_(const QDomElement & element);
    WOGBallShape* StringToShape(const QString & shape);
};

#endif // OG_BALLCONFIG_H
