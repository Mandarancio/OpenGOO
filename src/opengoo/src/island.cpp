#include "island.h"
#include "GameEngine/og_gameengine.h"
#include <OGPushButton>
#include "opengoo.h"
#include "uidata.h"
#include "og_utils.h"

#include <QString>

using namespace og;

struct Island::Impl
{
    std::unique_ptr<ui::IPushButton> pBackBtn;
};

Island::Island(const QString& /*name*/) : _pImpl(new Impl)
{
//    getGame()->_LoadLevel(name);

    auto data = utils::getUIData("BACK_BUTTON");
    int x = 20;
    int y = GE->getHeight() - (data->height + 20);
    _pImpl->pBackBtn.reset(utils::createPushButton(QPoint(x ,y), *data).release());
    auto btn = _pImpl->pBackBtn.get();
    connect(btn, SIGNAL(pressed()), this, SIGNAL(close()));
    btn->setVisible(true);
}

Island::~Island()
{
}

