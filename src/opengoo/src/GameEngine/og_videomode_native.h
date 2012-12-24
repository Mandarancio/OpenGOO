/*******************************************************************************
*  file    : og_videomode_native.h
*  created : 24.12.2012
*  author  : 
*******************************************************************************/

#ifndef OG_VIDEOMODE_NATIVE_H
#define OG_VIDEOMODE_NATIVE_H

#include "og_videomode.h"

OGVideoMode native_getCurrentMode();
bool        native_setVideoMode  (int width, int height);
bool        native_testVideoMode (int width, int height);

#endif // OG_VIDEOMODE_NATIVE_H
