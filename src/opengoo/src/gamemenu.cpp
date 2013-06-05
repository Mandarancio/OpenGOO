#include "gamemenu.h"
#include "og_data.h"
#include "og_utils.h"

#include <QPixmap>

using namespace og::ui;
using namespace ogUtils;
using namespace std;

struct GameMenu::Impl
{
    unique_ptr<IPushButton> restart;
    unique_ptr<IPushButton> resume;
    unique_ptr<IPushButton> backto;
    unique_ptr<IPushButton> ocdcriteria;
};

GameMenu::GameMenu() : _pImpl(new Impl)
{
    unique_ptr<WOGScene> scene(
                OGData::GetScene("./res/scenes/GameMenu.scene.xml"));
    unique_ptr<WOGResources> resrc(
                OGData::GetResources("./res/scenes/GameMenu.resrc.xml"));

    auto group = scene->GetButtonGroup("gamemenugroup");

    foreach (WOGButton* btn, group->button)
    {
        if (btn->onclick == "restartlevelrightnow")
        {
            _pImpl->restart = _createButton(btn, *resrc);
            auto ibtn = _pImpl->restart.get();
            connect(ibtn, SIGNAL(pressed()), this, SLOT(_restart()));
            ibtn->setVisible(true);
        }
        else if (btn->onclick == "showocdcriteria")
        {
            _pImpl->ocdcriteria = _createButton(btn, *resrc);
            auto ibtn = _pImpl->ocdcriteria.get();
            connect(ibtn, SIGNAL(pressed()), this, SLOT(_ocdcriteria()));
            ibtn->setVisible(true);
        }
        else if (btn->onclick == "backtoisland")
        {
            _pImpl->backto = _createButton(btn, *resrc);
            auto ibtn = _pImpl->backto.get();
            connect(ibtn, SIGNAL(pressed()), this, SIGNAL(backToIsland()));
            ibtn->setVisible(true);
        }
        else if (btn->onclick == "resumegame")
        {
            _pImpl->resume = _createButton(btn, *resrc);
            auto ibtn = _pImpl->resume.get();
            connect(ibtn, SIGNAL(pressed()), this, SIGNAL(close()));
            ibtn->setVisible(true);
        }
    }
}

GameMenu::~GameMenu() {}

unique_ptr<PushButton> GameMenu::_createButton(const WOGButton* button
                                    , const WOGResources &resrc)
{
    auto pm = _getImage(button->up, resrc);

    int w = pm->width() * button->scale.x();
    int h = pm->height() * button->scale.y();

    auto ge = getGameEngine();

    int x = ge->getWidth() / 2 + (button->position.x() - w / 2);
    int y = ge->getHeight() / 2 - (button->position.y() + h / 2);

    unique_ptr<PushButton> btn(new PushButton);

    btn->setPosition(x, y);
    btn->setSize(w, h);
    btn->setUpImage(pm);
    btn->setOverImage(_getImage(button->over, resrc));
    btn->setText(getText(button->text));

    return btn;
}

inline QPixmap* GameMenu::_getImage(const QString &id
                                    , const WOGResources &resrc)
{
    return new QPixmap(resrc.GetImage(id));
}

void GameMenu::_restart()
{
    getGame()->ReloadLevel();
    emit close();
}

void GameMenu::_ocdcriteria() { emit close(); }
