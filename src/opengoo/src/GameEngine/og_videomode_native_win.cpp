/*******************************************************************************
*  file    : og_videomode_native_win.cpp
*  created : 24.12.2012
*  author  : 
*******************************************************************************/

#include "og_videomode_native.h"
#include <windows.h>

OGVideoMode native_getCurrentMode()
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

bool native_setVideoMode(int width, int height)
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

bool native_testVideoMode(int width, int height)
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

