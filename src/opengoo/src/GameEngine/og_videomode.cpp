#include "og_videomode_native.h"


OGVideoMode OGVideoMode::getCurrentMode()
{
    return native_getCurrentMode();
}

bool OGVideoMode::testVideoMode(int width, int height)
{
    return native_testVideoMode(width, height);
}

bool OGVideoMode::setVideoMode(int width, int height)
{
    return native_setVideoMode(width, height);
}
