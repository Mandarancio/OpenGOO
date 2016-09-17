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
#include "og_world.h"
#include "wog_resources.h"
#include "wog_text.h"


#include <QFile>

void ogUtils::ogBackTracer()
{
#ifndef Q_OS_WIN32
    BackTracer(SIGSEGV);
    BackTracer(SIGFPE);
#endif

#ifdef Q_CC_MSVC
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

bool ogUtils::ogLoadConfig(OGConfig& config, const QString& filename)
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

void ogUtils::ogSaveConfig(OGConfig& config, const QString & filename)
{
    OGGameConfig gameConfig(filename);
    gameConfig.Create(config);
}

OGUserData* ogUtils::ogGetUserData(void* userdata)
{
    OGUserData* data = 0;

    if (userdata) data = static_cast<OGUserData*>(userdata);

    return data;
}

QPixmap* ogUtils::getImage(const QString & id)
{
    OGWorld* world = OpenGOO::instance()->GetWorld();
    WOGResources* resrc = world->resrcdata();

    return new QPixmap(resrc->GetImage(id));
}

QString ogUtils::getText(const QString & id)
{
    OGWorld* world = OpenGOO::instance()->GetWorld();
    WOGText* text = world->textdata();

    return text->GetString(id);
}

std::unique_ptr<UIData> ogUtils::getUIData(const QString & id)
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

template<class T> T* ogUtils::createUI(const QPoint & pos, const UIData & data)
{
    using namespace og::ui;

    T* ui = new T;

    ui->setPosition(pos.x(), pos.y());
    ui->setSize(data.width, data.height);
    ui->setUpImage(getImage(data.up));
    ui->setOverImage(getImage(data.over));
    ui->setText(getText(data.text));

    return ui;
}

og::ui::PushButton* ogUtils::createButton(const QPoint &pos
                                          , const UIData &data)
{
    using namespace og::ui;

    return createUI<PushButton> (pos, data);
}

