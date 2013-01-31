#include "wog_level.h"

WOGCamera::~WOGCamera()
{
    while(poi.isEmpty()) { delete poi.takeFirst(); }
}

WOGLevel::~WOGLevel()
{
    while (!camera.isEmpty()) { delete camera.takeFirst(); }

    while (!ball.isEmpty()) { delete ball.takeFirst(); }

    while (!strand.isEmpty()) { delete strand.takeFirst(); }

    delete levelexit;
}
