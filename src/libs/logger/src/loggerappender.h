/*******************************************************************************
*  file    : loggerappender.h
*  created : 22.11.2012
*  author  : 
*******************************************************************************/

#ifndef LOGGERAPPENDER_H
#define LOGGERAPPENDER_H

#include "logger.h"

class LoggerAppender
{
private:
    QString _format;
    LoggerEngine::LoggerLevel _level;

protected:
    QMutex mutex;

protected:
    LoggerAppender        (const QString &format, int level);
    void streamWrite      (QTextStream &stream, const LoggerEvent &event);
    virtual QString format(const LoggerEvent &event);

public:
    virtual void write (const LoggerEvent &event) = 0;
    LoggerEngine::LoggerLevel level();
    virtual ~LoggerAppender(){}
};

#endif // LOGGERAPPENDER_H
