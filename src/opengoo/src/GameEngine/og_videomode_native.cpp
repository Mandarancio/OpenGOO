/*******************************************************************************
*  file    : og_videomode_native.cpp
*  created : 24.12.2012
*  author  : 
*******************************************************************************/
#include <qglobal.h>

#ifdef Q_OS_LINUX
#include "og_videomode_native_linux.cpp"
#endif

#ifdef Q_OS_WIN
#include "og_videomode_native_win.cpp"
#endif
