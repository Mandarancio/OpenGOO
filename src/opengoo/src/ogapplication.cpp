#include "ogapplication.h"

#include <QApplication>
#include <QDebug>
#include <QDir>

#include "GameEngine/og_gameengine.h"
#include "opengoo.h"
#include "og_config.h"
#include "resourcemanagerfactory.h"

#include "flags.h"
#include "og_utils.h"

using namespace og;

const char* OGApplication::DEFAUL_LANGUAGE = "en";
const char* OGApplication::DEFAUL_LEVEL_NAME = "MapWorldView";

const char* OGApplication::RESOURCES_DIR = "res";
const char* OGApplication::PROPERTIES_DIR = "properties";
const char* OGApplication::FILE_CONFIG = "config.txt";

int OGApplication::run(int argc, char **argv)
{
    OGConfig config;
    if (initialize(argc, argv, config))
    {
        QApplication app(argc, argv);

        auto game = OpenGOO::GetInstance();
        game->GotoScene(config.levelName);
        game->SetLanguage(config.language);

        ResourceManagerFactory factory;
        OGGameEngine engine(game, config, factory);
        if (engine.initialize())
        {
            try
            {
                app.exec();
            }
            catch (const std::exception& ex)
            {
                logException(ex.what());
            }
        }

        game->Destroy();
    }

    return 0;
}

bool OGApplication::initialize(int argc, char **argv, OGConfig& config)
{
    utils::backTracer();
    utils::logger();

    bool isFullScreen = false;

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
            config.isCrt = true;
        }
        if (!arg.compare("--fullscreen", Qt::CaseInsensitive))
        {
            isFullScreen = true;
        }
        else if (!arg.compare("--level", Qt::CaseInsensitive))
        {
            if (++i < argc)
            {
                config.levelName = argv[i];
            }
        }
    }

    //CHECK FOR GAME DIR IN HOME DIRECTORY
    QDir dir;
    const auto gameDir = QDir::homePath() + "/.OpenGOO";
    //If the game dir doesn't exist create it
    if (!dir.exists(gameDir))
    {
        if (flag & DEBUG)
        {
            logWarn("Game dir doesn't exist!");
        }

        dir.mkdir(gameDir);
        dir.cd(gameDir);
        //create subdir for user levels and progressions.
        dir.mkdir("userLevels");
        dir.mkdir("userProgression");
        dir.mkdir("debug");
    }
    else if (flag & DEBUG)
    {
        logWarn("Game dir exist!");
    }

    if (!dir.exists(RESOURCES_DIR))
    {
        logError(QString(RESOURCES_DIR) + " directory not found");
        return false;
    }

    if (!dir.exists(PROPERTIES_DIR))
    {
        logError(QString(PROPERTIES_DIR) + " directory not found");
        return false;
    }

    auto path = QString("%1/%2").arg(PROPERTIES_DIR).arg(FILE_CONFIG);
    if (!utils::loadConfig(config, path))
    {
        logWarn(QString("Could not load config file:") + FILE_CONFIG);
        config.fullscreen = false;
        config.screen_width = DEFAUL_SCREEN_WIDTH;
        config.screen_height = DEFAUL_SCREEN_HEIGHT;
        config.refreshrate = DEFAUL_FRAMERATE;
        config.language = DEFAUL_LANGUAGE;

        logInfo("Saving config file...");
        utils::saveConfig(config, path);
    }

    if (config.refreshrate == 0)
    {
        config.refreshrate = DEFAUL_FRAMERATE;
    }

    if (isFullScreen)
    {
        config.fullscreen = true;
    }

    if (config.levelName.isEmpty())
    {
        config.levelName = DEFAUL_LEVEL_NAME;
    }

    return true;
}
