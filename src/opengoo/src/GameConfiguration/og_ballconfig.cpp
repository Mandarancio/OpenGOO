#include "og_ballconfig.h"

#include <QDebug>

OGBallConfig::OGBallConfig(const QString &filename)
    : OGXmlConfig(filename)
{
    SetRootTag("ball");
}

WOGBall* OGBallConfig::Parser()
{
    qWarning() << __FUNCTION__ << "is UNIMPLEMENTED!";

    return 0;
}
