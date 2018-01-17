#pragma once

#include "og_xmlconfig.h"
#include "wog_ball.h"

class OGBallConfig : public OGXmlConfig
{
public:
    OGBallConfig()
    {
        SetRootTag("ball");
    }

    WOGBall* Parser();

private:
    void CreateAttributes_(WOGBall* ball);
    void CreateCoreAttributes_(WOGBall* ball);
    void CreateBehaviourAttributes_(WOGBall* ball);
    void CreateMovementAttributes_(WOGBall* ball);
    void CreatePlayerInteraction_(WOGBall* ball);
    void CreateCosmeticAttributes_(WOGBall* ball);
    void _CreateLevelInteraction(WOGBall* ball);
    void LoadPartAttr(const QString& a_attrName, const QString& a_attrValue, WOGPart& a_part);
    void LoadPart(const QDomElement& a_el, WOGPart& a_part);
    WOGBallStrand* CreateStrand_(const QDomElement & element);
    WOGBallDetachstrand* CreateDetachstrand_(const QDomElement & element);
    std::shared_ptr<WOGBallShape> StringToShape(const QString & shape);
};
