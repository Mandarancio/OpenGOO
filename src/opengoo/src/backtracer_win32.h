/*
* file   : backtracer_win32.h created 2012.6.25
* class  : backtracer_win32
* author : Slyshyk Oleksiy ( alexSlyshyk@gmail.com )
*/

#ifndef BACKTRACER_WIN32_H
#define BACKTRACER_WIN32_H
#include <Windows.h>

LONG WINAPI UnhandledException(LPEXCEPTION_POINTERS exceptionInfo);
LONG WINAPI UnhandledException2(LPEXCEPTION_POINTERS exceptionInfo);

#endif // BACKTRACER_WIN32_H
