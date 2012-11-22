/*******************************************************************************
*  file    : loggerappender.cpp
*  created : 22.11.2012
*  author  : 
*******************************************************************************/

#include "loggerappender.h"
#include "loggerevent.h"

void LoggerAppender::streamWrite (QTextStream &stream, const LoggerEvent &event)
{
    stream << format(event);
    stream.flush();
}

QString LoggerAppender::format(const LoggerEvent &event)
{
    QString message = QString(_format);
    message.replace("%d", event.dateTime.toString("hh:mm:ss.zzz"));
    message.replace("%l", LoggerEngine::levelName(event.level));
    message.replace("%i", QString("%1").arg(event.line));
    message.replace("%f", QString(event.file));
    message.replace("%m", *(event.message));
    message.replace("%n", "\n");

    return message;
}

LoggerAppender::LoggerAppender (const QString &format, int level)
{
    _level = (LoggerEngine::LoggerLevel) level;
    if (format.isNull())
        _format = "%d - <%l> - {%c} -- %m%n";
    else
        _format = format;
}

LoggerEngine::LoggerLevel LoggerAppender::level ()
{
    return _level;
}
