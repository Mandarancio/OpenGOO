#include "og_videomode.h"

#include <windows.h>

OGVideoMode OGVideoMode::getCurrentMode()
{
    DEVMODE DevMode;
    DevMode.dmSize = sizeof(DevMode);
    DevMode.dmDriverExtra  = 0;

    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DevMode);

    return OGVideoMode(DevMode.dmPelsWidth
                           , DevMode.dmPelsHeight
                           , DevMode.dmBitsPerPel
                           , DevMode.dmDisplayFrequency);
}

bool OGVideoMode::testVideoMode(int width, int height)
{
    DEVMODE DevMode;
    DevMode.dmSize = sizeof(DevMode);
    DevMode.dmDriverExtra  = 0;

    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DevMode);

    DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
    DevMode.dmPelsWidth = width;
    DevMode.dmPelsHeight = height;

    LONG Status = ChangeDisplaySettings(&DevMode, CDS_TEST);

    if (Status == DISP_CHANGE_SUCCESSFUL) { return true; }
    else { return false; }
}

bool OGVideoMode::setVideoMode(int width, int height)
{
    DEVMODE DevMode;
    DevMode.dmSize = sizeof(DevMode);
    DevMode.dmDriverExtra  = 0;

    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DevMode);

    DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
    DevMode.dmPelsWidth = width;
    DevMode.dmPelsHeight = height;

    LONG Status = ChangeDisplaySettings(&DevMode, 0);

    if (Status == DISP_CHANGE_SUCCESSFUL) { return true; }
    else { return false; }
}
