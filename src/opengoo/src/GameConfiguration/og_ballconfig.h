#ifndef OG_BALLCONFIG_H
#define OG_BALLCONFIG_H

#include "og_xmlconfig.h"
#include "wog_ball.h"


class OGBallConfig : public OGXmlConfig
{
public:
    OGBallConfig(const QString & filename);

    WOGBall* Parser();
};

#endif // OG_BALLCONFIG_H
