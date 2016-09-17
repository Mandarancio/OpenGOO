#include "ogapplication.h"
#include "GameEngine/og_gameengine.h"
#include "opengoo.h"

#include "flags.h"
#include "og_utils.h"

#include <QDir>
#include <QApplication>
#include <QDebug>

using namespace og;

int OGApplication::run(int argc, char **argv)
{
    if (initialize(argc, argv))
    {
        QApplication app(argc, argv);

        if (OGGameEngine::getInstance()->initialize()) app.exec();
    }

    clear();

    return 0;
}

bool OGApplication::initialize(int argc, char **argv)
{
    const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";
    const QString RESOURCES_DIR = "res";
    const QString PROPERTIES_DIR = "properties";
    const QString FILE_CONFIG = "config.txt";
    const int FRAMERATE = 60;

    ogUtils::ogBackTracer();
    ogUtils::ogLogger();

    QString levelName;
    bool isCrt = false;

    //Check for the run parameters
    for (int i = 1; i < argc; i++)
    {
        QString arg(argv[i]);
        //Check for debug Option
        if (!arg.compare("--debug", Qt::CaseInsensitive))
        {
            flag |= DEBUG;
            logWarn("DEBUG MODE ON");
        }
        if (!arg.compare("--fps", Qt::CaseInsensitive))
        {
            flag |= FPS;
        }
        if (!arg.compare("--crt", Qt::CaseInsensitive))
        {
            isCrt = true;
        }
        else if (!arg.compare("--level", Qt::CaseInsensitive))
        {
            if (++i < argc)
            {
                levelName = QString(argv[i]);
            }
        }
    }

    //CHECK FOR GAME DIR IN HOME DIRECTORY
    QDir dir;
    //If the game dir doesn't exist create it
    if (!dir.exists(GAMEDIR))
    {
        if (flag & DEBUG) logWarn("Game dir doesn't exist!");
        dir.mkdir(GAMEDIR);
        dir.cd(GAMEDIR);
        //create subdir for user levels and progressions.
        dir.mkdir("userLevels");
        dir.mkdir("userProgression");
        dir.mkdir("debug");
    }
    else if (flag & DEBUG) logWarn("Game dir exist!");

    if (!dir.exists(RESOURCES_DIR))
    {
        logError(RESOURCES_DIR + " directory not found");
        return false;
    }

    if (!dir.exists(PROPERTIES_DIR))
    {
        logError(PROPERTIES_DIR + " directory not found");
        return false;
    }

    OGConfig config;
    auto isLoaded = ogUtils::ogLoadConfig(config, PROPERTIES_DIR + "/" + FILE_CONFIG);
    if (!isLoaded)
    {
        logWarn("Could not load config file:" + FILE_CONFIG);
        config.fullscreen = false;
        config.screen_width = 800;
        config.screen_height = 600;
        config.refreshrate = 60;
        config.language = "en";

        logInfo("Saving config file...");
        ogUtils::ogSaveConfig(config, PROPERTIES_DIR + "/" + FILE_CONFIG);
    }

    int refreshrate = config.refreshrate;

    if (refreshrate == 0)
        refreshrate = FRAMERATE;

    auto game = OpenGOO::instance();
    game->SetLevelName(levelName);
    game->SetLanguage(config.language);

    auto engine = new OGGameEngine(game, config, isCrt);
    (void) engine;

    return true;
}

void OGApplication::clear()
{
    OpenGOO::instance()->Destroy();
    delete OGGameEngine::getInstance();
}
