#include "wog_level.h"

WOGCamera::~WOGCamera()
{
    while(!poi.isEmpty()) { delete poi.takeFirst(); }
}

WOGLevel::~WOGLevel()
{
    while (!camera.isEmpty()) { delete camera.takeFirst(); }
    while (!ball.isEmpty()) { delete ball.takeFirst(); }
    while (!strand.isEmpty()) { delete strand.takeFirst(); }

    delete levelexit;
    delete pipe;
}

WOGCamera* WOGLevel::GetCameraByAspect(const QString& aspect) const
{
    Q_FOREACH(WOGCamera* cam, camera)
    {
        if (cam->aspect == aspect)
        {
            return cam;
        }
    }

    return 0;
}
