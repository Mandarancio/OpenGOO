#include "level.h"
#include "retrymenu.h"
#include "gamemenu.h"
#include "GameEngine/og_gameengine.h"
#include <OGPushButton>
#include "opengoo.h"
#include "uidata.h"
#include "og_utils.h"

#include <QString>

using namespace og;

struct Level::Impl
{    
    ui::IPushButtonUPtr menuBtn;
    ui::IPushButtonUPtr retryBtn;
    std::unique_ptr<GameMenu> gameMenu;
    std::unique_ptr<RetryMenu> retryMenu;
};

Level::Level(const QString &/*levelname*/) : mImpl(new Impl)
{    
//    getGame()->_LoadLevel(levelname);

    // Creates the menu button
    {
        auto data = utils::getUIData("MENU_BUTTON");
        int x = GE->getWidth() - (data->width + 20);
        int y = GE->getHeight() - (data->height + 20);
        mImpl->menuBtn.reset(utils::createPushButton(QPoint(x,y), *data).release());
        auto btn = mImpl->menuBtn.get();
        connect(btn, SIGNAL(pressed()), this, SLOT(menuButton()));
    }

    // Creates the retry button
    {
        auto data = utils::getUIData("RETRY_BUTTON");
        int x = 20;
        int y = GE->getHeight() - (data->height + 20);
        mImpl->retryBtn.reset(utils::createPushButton(QPoint(x,y), *data).release());
        auto btn = mImpl->retryBtn.get();
        connect(btn, SIGNAL(pressed()), this, SLOT(retryButton()));
    }

    showButton();
}

Level::~Level()
{
    GAME->_CloseLevel();
}

void Level::retryButton()
{
    GAME->SetPause(true);
    hideButton();
    mImpl->retryMenu.reset(new RetryMenu);
    auto menu = mImpl->retryMenu.get();
    connect(menu, SIGNAL(close()), this, SLOT(retryMenu()));
}

void Level::retryMenu()
{
    mImpl->retryMenu.reset();
    showButton();
    GAME->SetPause(false);
}

void Level::menuButton()
{
    GAME->SetPause(true);
    hideButton();
    mImpl->gameMenu.reset(new GameMenu);
    auto menu = mImpl->gameMenu.get();
    connect(menu, SIGNAL(close()), this, SLOT(gameMenu()));
    connect(menu, SIGNAL(backToIsland()), this, SIGNAL(closeLevel()));
}

void Level::gameMenu()
{
    mImpl->gameMenu.reset();
    showButton();
    GAME->SetPause(false);
}

void Level::hideButton()
{
    mImpl->menuBtn->setVisible(false);
    mImpl->retryBtn->setVisible(false);
}

void Level::showButton()
{
    mImpl->menuBtn->setVisible(true);
    mImpl->retryBtn->setVisible(true);
}
