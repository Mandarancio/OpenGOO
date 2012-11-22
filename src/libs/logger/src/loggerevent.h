/*******************************************************************************
*  file    : loggerevent.h
*  created : 22.11.2012
*  author  : 
*******************************************************************************/

#ifndef LOGGEREVENT_H
#define LOGGEREVENT_H

#include "logger.h"
#include <QDateTime>

class LoggerEvent
{
public:
    LoggerEngine::LoggerLevel level;
    QDateTime     dateTime;
    const QString *message;
    const char    *file;
    int line;
public:
    LoggerEvent() : message(NULL) {}
};

#endif // LOGGEREVENT_H
