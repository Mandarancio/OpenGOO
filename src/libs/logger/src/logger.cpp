/*******************************************************************************
*  file    : logger.cpp
*  created : 21.11.2012
*  author  : 
*******************************************************************************/

#include "logger.h"
#include "loggerevent.h"
#include "loggerappender.h"


bool LoggerEngine::isEventAccepted (LoggerAppender *appender, LoggerEvent &event)
{
    if (!(appender->level() & event.level))
       return false;
    return true;
}

void LoggerEngine::logEvent (LoggerLevel level,
                             const QString &message, const char *file, int line)
{
    LoggerEvent event;
    event.dateTime = QDateTime::currentDateTime();
    event.level    = level;
    event.message  = &message;
    event.file     = file;
    event.line     = line;

    foreach (LoggerAppender *appender, handle().appenders_ )
        {
            if (isEventAccepted(appender, event) == true)
                {
                    appender->write(event);
                }
        }
}

const char * LoggerEngine::levelName (LoggerLevel level)
{
    switch (level)
        {
        case LevelDebug:     return "Debug";
        case LevelInfo:      return "Info";
        case LevelWarn:      return "Warn";
        case LevelError:     return "Error";
        case LevelCritical:  return "Critical";
        case LevelFatal:     return "Fatal";
        case LevelException: return "Exception";
        default:
            return "Unknown";
        }
}

LoggerEngine::~LoggerEngine ()
{
    foreach (LoggerAppender *appender, appenders_)
        {
            delete appender;
        }
}


void LoggerEngine::addAppender(LoggerAppender *appender)
{
    if (appender)
        {
            handle().appenders_ << appender;
        }
}
