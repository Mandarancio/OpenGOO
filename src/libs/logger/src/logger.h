/*******************************************************************************
*  file    : logger.h
*  created : 21.11.2012
*  author  : 
*******************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QMutex>
#include <QHash>
#include <QDateTime>
#include <QTextStream>

class LoggerAppender;
class LoggerEvent;

class LoggerEngine
{

private:
    friend class LoggerAppender;
    QTime                   startTime_;
    QList<LoggerAppender *> appenders_;

private:
    static LoggerEngine &handle()
        {
            static LoggerEngine hInstance;
            if (hInstance.startTime_.isNull())
                hInstance.startTime_.start();
            return hInstance;
        }
    ~LoggerEngine ();

public:
    typedef enum {
        LevelDebug = 1,
        LevelInfo = 2,
        LevelWarn = 4,
        LevelError = 8,
        LevelCritical = 16,
        LevelFatal = 32,
        LevelException = 64,
        LevelAll = 127
    } LoggerLevel;

public:
    static void     addAppender(LoggerAppender *appender);
    static void     logEvent        (LoggerLevel level,
                                     const QString &message,
                                     const char *file,
                                     int line);
    static bool     isEventAccepted (LoggerAppender *appender, LoggerEvent &event);

    static const char *levelName(LoggerLevel level);
};


#define logDebug(message)     LoggerEngine::logEvent(LoggerEngine::LevelDebug, message, __FILE__, __LINE__)
#define logInfo(message)      LoggerEngine::logEvent(LoggerEngine::LevelInfo, message, __FILE__, __LINE__)
#define logWarn(message)      LoggerEngine::logEvent(LoggerEngine::LevelWarn, message, __FILE__, __LINE__)
#define logError(message)     LoggerEngine::logEvent(LoggerEngine::LevelError, message, __FILE__, __LINE__)
#define logFatal(message)     LoggerEngine::logEvent(LoggerEngine::LevelFatal, message, __FILE__, __LINE__)
#define logCritical(message)  LoggerEngine::logEvent(LoggerEngine::LevelCritical, message, __FILE__, __LINE__)
#define logException(message) LoggerEngine::logEvent(LoggerEngine::LevelException, message, __FILE__, __LINE__)


#endif // LOGGER_H
