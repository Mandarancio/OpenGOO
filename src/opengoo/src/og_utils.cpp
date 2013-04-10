#include "og_utils.h"

#ifndef Q_OS_WIN32
#include "backtracer.h"
#endif

#ifdef Q_OS_WIN32
#include "backtracer_win32.h"
#endif


#include <QFile>

void ogUtils::ogBackTracer()
{
#ifndef Q_OS_WIN32
    BackTracer(SIGSEGV);
    BackTracer(SIGFPE);
#endif

#ifdef Q_OS_WIN32
    AddVectoredExceptionHandler(0, UnhandledException2);
#endif
}

void ogUtils::ogMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type)
    {
        case QtDebugMsg:
            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            abort();
    }
}

void ogUtils::ogLogger()
{
    ConsoleAppender* con_apd;
    con_apd  = new ConsoleAppender(LoggerEngine::LevelInfo, stdout, "%d - <%l> - %m%n");
    LoggerEngine::addAppender(con_apd);
    con_apd  = new ConsoleAppender(LoggerEngine::LevelDebug     |
                                   LoggerEngine::LevelWarn      |
                                   LoggerEngine::LevelCritical  |
                                   LoggerEngine::LevelError     |
                                   LoggerEngine::LevelException |
                                   LoggerEngine::LevelFatal, stdout, "%d - <%l> - %m [%f:%i]%n");
    LoggerEngine::addAppender(con_apd);
    qInstallMessageHandler(ogMessageHandler);
}

OGConfig ogUtils::ogReadConfig(const QString & filename)
{
    // Read game configuration

    QFile::exists(filename);

    OGGameConfig gameConfig(filename);

    OGConfig config;

    if (gameConfig.Open())
    {
        if (gameConfig.Read()) { config = gameConfig.Parser(); }
        else {logWarn("File " + filename + " is corrupted"); }
    }
    else
    {
        logWarn("File " + filename + " not found");
        gameConfig.Create(config);
    }

    return config;
}
