#include "level.h"
#include "retrymenu.h"
#include "gamemenu.h"
#include "og_utils.h"

#include <QString>

using namespace og::ui;
using namespace ogUtils;
using namespace std;

struct Level::Impl
{    
    unique_ptr<IPushButton> pMenuBtn;
    unique_ptr<IPushButton> pRetryBtn;
    unique_ptr<GameMenu> pGameMenu;
    unique_ptr<RetryMenu> pRetryMenu;
};

Level::Level(const QString &levelname) : _pImpl(new Impl)
{    
    getGame()->_LoadLevel(levelname);

    auto ge = getGameEngine();

    // Creates the menu button
    {
        auto data = getUIData("MENU_BUTTON");
        int x = ge->getWidth() - (data->width + 20);
        int y = ge->getHeight() - (data->height + 20);
        _pImpl->pMenuBtn.reset(createButton(QPoint(x,y), *data));
        auto btn = _pImpl->pMenuBtn.get();
        connect(btn, SIGNAL(pressed()), this, SLOT(_menuButton()));        
    }

    // Creates the retry button
    {
        auto data = getUIData("RETRY_BUTTON");
        int x = 20;
        int y = ge->getHeight() - (data->height + 20);
        _pImpl->pRetryBtn.reset(createButton(QPoint(x,y), *data));
        auto btn = _pImpl->pRetryBtn.get();
        connect(btn, SIGNAL(pressed()), this, SLOT(_retryButton()));
    }

    showButton();
}

Level::~Level()
{
    getGame()->_CloseLevel();
    getGame()->_ClearSelectedBall();
}

void Level::_retryButton()
{
    getGame()->SetPause(true);
    hideButton();
    _pImpl->pRetryMenu.reset(new RetryMenu);
    auto menu = _pImpl->pRetryMenu.get();
    connect(menu, SIGNAL(close()), this, SLOT(_retryMenu()));
}

void Level::_retryMenu()
{
    _pImpl->pRetryMenu.reset();
    showButton();
    getGame()->SetPause(false);
}

void Level::_menuButton()
{
    getGame()->SetPause(true);
    hideButton();
    _pImpl->pGameMenu.reset(new GameMenu);
    auto menu = _pImpl->pGameMenu.get();
    connect(menu, SIGNAL(close()), this, SLOT(_gameMenu()));
    connect(menu, SIGNAL(backToIsland()), this, SIGNAL(closeLevel()));
}

void Level::_gameMenu()
{
    _pImpl->pGameMenu.reset();
    showButton();
    getGame()->SetPause(false);
}

void Level::hideButton()
{
    _pImpl->pMenuBtn->setVisible(false);
    _pImpl->pRetryBtn->setVisible(false);
}

void Level::showButton()
{
    _pImpl->pMenuBtn->setVisible(true);
    _pImpl->pRetryBtn->setVisible(true);
}
