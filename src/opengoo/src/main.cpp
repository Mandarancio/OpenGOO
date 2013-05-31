/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "GameEngine/og_gameengine.h"
#include "opengoo.h"

#include "flags.h"
#include "og_utils.h"

#include <QDir>
#include <QApplication>

bool Initialize(int argc, char** argv);
void Clear();

using namespace og;

int main(int argc, char** argv)
{
    if (Initialize(argc, argv))
    {
        QApplication app(argc, argv);

        if (OGGameEngine::getEngine()->initialize()) app.exec();
    }

    Clear();

    return 0;
}

bool Initialize(int argc, char** argv)
{
    const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";
    const QString FILE_CONFIG = "./resources/config.xml";
    const int FRAMERATE = 60;

    ogUtils::ogBackTracer();
    ogUtils::ogLogger();

    QString levelName;

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
        if (!arg.compare("--fps", Qt::CaseInsensitive)) { flag |= FPS; }
        else if (!arg.compare("--level", Qt::CaseInsensitive))
        {
            if (++i < argc) { levelName = QString(argv[i]); }
        }
    }

    //CHECK FOR GAME DIR IN HOME DIRECTORY
    QDir dir(GAMEDIR);
    //If the game dir doesn't exist create it
    if (!dir.exists())
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

    OGConfig config = ogUtils::ogReadConfig(FILE_CONFIG);

    OGGameEngine* gameEngine = 0;
    OpenGOO* game = OpenGOO::instance();
    game->SetLevelName(levelName);
    game->SetLanguage(config.language);

#ifdef Q_OS_WIN32
    gameEngine = new OGGameEngine(game, config.screen_width
                                  , config.screen_height
                                  , config.fullscreen);
#else
    gameEngine = new OGGameEngine(game, config.screen_width
                                  , config.screen_height
                                  , false);
#endif //   Q_OS_WIN32

    if (gameEngine == 0) { return false; }

    int refreshrate = config.refreshrate;

    if (refreshrate == 0) refreshrate = FRAMERATE;

    gameEngine->setFrameRate(refreshrate);

    return true;
}

void Clear()
{
    OpenGOO::instance()->Destroy();
    delete OGGameEngine::getEngine();
}
