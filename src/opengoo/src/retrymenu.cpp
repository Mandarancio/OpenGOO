#include "retrymenu.h"
#include "GameEngine/og_gameengine.h"
#include <OGPushButton>
#include "opengoo.h"
#include "uidata.h"
#include "og_utils.h"

using namespace og;

struct RetryMenu::Impl
{
    ui::IPushButtonUPtr okBtn;
    ui::IPushButtonUPtr cancelBtn;
};

RetryMenu::RetryMenu() : mImpl(new Impl)
{    
    {
        auto data = utils::getUIData("OK_BUTTON");
        auto offset = 10 + data->width;
        auto pos = QPoint(GE->getWidth() / 2 - offset, GE->getHeight() / 2);
        mImpl->okBtn = std::move(utils::createPushButton(pos, *data));
        auto btn = mImpl->okBtn.get();
        connect(btn, SIGNAL(pressed()), this, SLOT(ok()));
        btn->setVisible(true);
    }

    {
        auto data = utils::getUIData("CANCEL_BUTTON");
        auto offset = 10;
        auto pos = QPoint(GE->getWidth() / 2 + offset, GE->getHeight() / 2);
        mImpl->cancelBtn = std::move(utils::createPushButton(pos, *data));
        auto btn = mImpl->cancelBtn.get();
        connect(btn, SIGNAL(pressed()), this, SIGNAL(close()));
        btn->setVisible(true);
    }
}

RetryMenu::~RetryMenu()
{
}

void RetryMenu::ok()
{
    GAME->ReloadLevel();
    emit close();
}
