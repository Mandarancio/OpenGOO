#include "og_utils.h"

#ifndef Q_OS_WIN32
#include "backtracer.h"
#endif

#ifdef Q_OS_WIN32
#include "backtracer_win32.h"
#endif

#include "og_userdata.h"
#include <OGPushButton>
#include "opengoo.h"
#include "og_gameconfig.h"
#include "uidata.h"

#include <QFile>

namespace og
{
namespace utils
{

void backTracer()
{
#ifndef Q_OS_WIN32
    BackTracer(SIGSEGV);
    BackTracer(SIGFPE);
#endif

#ifdef Q_CC_MSVC
    AddVectoredExceptionHandler(0, UnhandledException2);
#endif
}

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
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

void logger()
{
    ConsoleAppender* con_apd;
    freopen ("log.txt", "w", stdout);
    con_apd  = new ConsoleAppender(LoggerEngine::LevelInfo, stdout, "%d - <%l> - %m%n");
    LoggerEngine::addAppender(con_apd);
    con_apd  = new ConsoleAppender(LoggerEngine::LevelDebug     |
                                   LoggerEngine::LevelWarn      |
                                   LoggerEngine::LevelCritical  |
                                   LoggerEngine::LevelError     |
                                   LoggerEngine::LevelException |
                                   LoggerEngine::LevelFatal, stdout, "%d - <%l> - %m [%f:%i]%n");
    LoggerEngine::addAppender(con_apd);
    qInstallMessageHandler(messageHandler);
}

bool loadConfig(OGConfig& config, const QString& filename)
{
    OGGameConfig gameConfig(filename);

    if (!gameConfig.Open())
    {
        logWarn("Could not open file:" + filename);
        return false;
    }

    if (!gameConfig.Read())
    {
        logWarn("Could not read file:" + filename);
        return false;
    }

    config = gameConfig.Parser();

    return true;
}

void saveConfig(OGConfig& config, const QString & filename)
{
    OGGameConfig gameConfig(filename);
    gameConfig.Create(config);
}

QPixmap* getImage(const QString & /*id*/)
{
    assert(false);
    return nullptr;
}

QString getText(const QString & /*id*/)
{
    assert(false);
    return "";
}

std::unique_ptr<UIData> getUIData(const QString & id)
{    
    QFile file(":/ui/resources.xml");
    file.open(QIODevice::ReadOnly);

    QDomDocument domDoc;
    std::unique_ptr<UIData> data;

    if (domDoc.setContent(&file))
    {
        QDomElement rootElement = domDoc.documentElement();
        QDomNode node = rootElement.firstChild();
        QDomElement element;
        bool isFound = false;

        while (!node.isNull() && !isFound)
        {
            element = node.toElement();

            if (element.attribute("id") == id) isFound = true;
            else node = node.nextSibling();
        }

        if (isFound)
        {
            data.reset(new UIData);
            data->width = element.attribute("width").toInt();
            data->height = element.attribute("height").toInt();
            data->up = element.attribute("up");
            data->over = element.attribute("over");
            data->text = element.attribute("text");
        }
    }

    return data;
}

std::unique_ptr<ui::PushButton> createPushButton(const QPoint &pos, const UIData& data)
{
    return createUI<ui::PushButton>(pos, data);
}

} // ns:utils
} // ns:og
