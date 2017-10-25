/*******************************************************************************
*  file    : og_videomode_native.h
*  created : 24.12.2012
*  author  : 
*******************************************************************************/

#pragma once

#include "og_videomode.h"

OGVideoMode native_getCurrentMode();
bool        native_returnDefaultMode();
bool        native_setVideoMode  (int width, int height);
bool        native_setVideoMode  (const OGVideoMode& mode);
bool        native_testVideoMode (int width, int height);
