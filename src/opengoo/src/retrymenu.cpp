#include "retrymenu.h"
#include "og_utils.h"

using namespace og::ui;
using namespace ogUtils;
using namespace std;

struct RetryMenu::Impl
{
    unique_ptr<IPushButton> okBtn;
    unique_ptr<IPushButton> cancelBtn;
};

RetryMenu::RetryMenu() : _pImpl(new Impl)
{    
    auto ge = getGameEngine();

    // Creates the OK button
    {
        auto data = getUIData("OK_BUTTON");
        int x = ge->getWidth() / 2 - (10 + data->width);
        int y = ge->getHeight() / 2;
        _pImpl->okBtn.reset(createButton(QPoint(x, y), *data));
        auto btn = _pImpl->okBtn.get();
        connect(btn, SIGNAL(pressed()), this, SLOT(_ok()));
        btn->setVisible(true);
    }

    // Creates the cancel button
    {
        auto data = getUIData("CANCEL_BUTTON");
        int x = ge->getWidth() / 2 + 10;
        int y = ge->getHeight() / 2;
        _pImpl->cancelBtn.reset(createButton(QPoint(x, y), *data));
        auto btn = _pImpl->cancelBtn.get();
        connect(btn, SIGNAL(pressed()), this, SIGNAL(close()));
        btn->setVisible(true);
    }
}

RetryMenu::~RetryMenu() {}

void RetryMenu::_ok()
{
    getGame()->ReloadLevel();
    emit close();
}
