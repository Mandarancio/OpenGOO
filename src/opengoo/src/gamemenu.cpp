#include "gamemenu.h"

#include <QPixmap>

#include "GameEngine/og_gameengine.h"

#include <OGPushButton>

#include "opengoo.h"
#include "og_data.h"
#include "uidata.h"
#include "og_utils.h"
#include "wog_resources.h"

using namespace og;

struct GameMenu::Impl
{
    ui::IPushButtonUPtr restart;
    ui::IPushButtonUPtr resume;
    ui::IPushButtonUPtr backto;
    ui::IPushButtonUPtr ocdcriteria;
};

struct GameMenuHelper
{
    static std::unique_ptr<QPixmap> getImage(const QString &id, const WOGResources &resrc)
    {
        return std::unique_ptr<QPixmap>(new QPixmap(resrc.GetImage(id)));
    }

    static ui::IPushButtonUPtr createButton(const WOGButton* button, const WOGResources &resrc)
    {
        auto pm = getImage(button->up, resrc);

        int w = pm->width() * button->scale.x();
        int h = pm->height() * button->scale.y();

        int x = GE->getWidth() / 2 + (button->position.x() - w / 2);
        int y = GE->getHeight() / 2 - (button->position.y() + h / 2);

        ui::IPushButtonUPtr btn = std::unique_ptr<ui::PushButton>(new ui::PushButton);
        btn->setPosition(x, y);
        btn->setSize(w, h);
        btn->setUpImage(pm.release());
        btn->setOverImage(getImage(button->over, resrc).release());
        btn->setText(utils::getText(button->text));

        return btn;
    }


    static std::unique_ptr<WOGScene> getScene()
    {
        return std::unique_ptr<WOGScene>(OGData::GetScene("./res/scenes/GameMenu.scene"));
    }

    static std::unique_ptr<WOGResources> getResources()
    {
        return std::unique_ptr<WOGResources>(OGData::GetResources("./res/scenes/GameMenu.resrc"));
    }
};

GameMenu::GameMenu() : mImpl(new Impl)
{
    auto scene = GameMenuHelper::getScene();
    auto group = scene->GetButtonGroup("gamemenugroup");
    auto resrc = GameMenuHelper::getResources();

    foreach (const WOGButton& btn, group->button)
    {
        if (btn.onclick == "restartlevelrightnow")
        {
            mImpl->restart = GameMenuHelper::createButton(&btn, *resrc);
            auto ibtn = mImpl->restart.get();
            connect(ibtn, SIGNAL(pressed()), this, SLOT(restart()));
            ibtn->setVisible(true);
        }
        else if (btn.onclick == "showocdcriteria")
        {
            mImpl->ocdcriteria = GameMenuHelper::createButton(&btn, *resrc);
            auto ibtn = mImpl->ocdcriteria.get();
            connect(ibtn, SIGNAL(pressed()), this, SLOT(ocdcriteria()));
            ibtn->setVisible(true);
        }
        else if (btn.onclick == "backtoisland")
        {
            mImpl->backto = GameMenuHelper::createButton(&btn, *resrc);
            auto ibtn = mImpl->backto.get();
            connect(ibtn, SIGNAL(pressed()), this, SIGNAL(backToIsland()));
            ibtn->setVisible(true);
        }
        else if (btn.onclick == "resumegame")
        {
            mImpl->resume = GameMenuHelper::createButton(&btn, *resrc);
            auto ibtn = mImpl->resume.get();
            connect(ibtn, SIGNAL(pressed()), this, SIGNAL(close()));
            ibtn->setVisible(true);
        }
    }
}

GameMenu::~GameMenu()
{
}

void GameMenu::restart()
{
    GAME->ReloadLevel();
    emit close();
}

void GameMenu::ocdcriteria()
{
    emit close();
}
