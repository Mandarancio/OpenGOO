/*******************************************************************************
*  file    : og_videomode_native_linux.cpp
*  created : 24.12.2012
*  author  : 
*******************************************************************************/
#include "og_videomode_native.h"

OGVideoMode native_getCurrentMode()
{
    return OGVideoMode();
}

bool native_setVideoMode  (int width, int height)
{
    return false;
}

bool native_testVideoMode (int width, int height)
{
    return false;
}
