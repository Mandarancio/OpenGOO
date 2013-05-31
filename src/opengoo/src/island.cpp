#include "island.h"
#include "og_utils.h"

#include <QString>

using namespace og::ui;
using namespace ogUtils;

struct Island::Impl
{
    std::unique_ptr<IPushButton> pBackBtn;
};

Island::Island(const QString &name) : _pImpl(new Impl)
{
    getGame()->_LoadLevel(name);

    auto data = getUIData("BACK_BUTTON");
    int x = 20;
    int y = getGameEngine()->getHeight() - (data->height + 20);
    _pImpl->pBackBtn.reset(createButton(QPoint(x ,y), *data));
    auto btn = _pImpl->pBackBtn.get();
    connect(btn, SIGNAL(pressed()), this, SIGNAL(close()));
    btn->setVisible(true);
}

Island::~Island() { getGame()->_CloseLevel(); }

