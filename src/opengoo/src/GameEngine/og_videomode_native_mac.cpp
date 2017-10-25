/*******************************************************************************
*  file    : og_videomode_native_mac.cpp
*  created : 22.10.2017
*  author  : 
*******************************************************************************/
#include "og_videomode_native.h"

OGVideoMode native_getCurrentMode()
{
    return OGVideoMode();
}

bool native_returnDefaultMode()
{
    return false;
}

bool native_setVideoMode  (int /*width*/, int /*height*/)
{
    return false;
}

bool native_setVideoMode(const OGVideoMode& /*mode*/)
{
    return false;
}

bool native_testVideoMode (int /*width*/, int /*height*/)
{
    return false;
}

