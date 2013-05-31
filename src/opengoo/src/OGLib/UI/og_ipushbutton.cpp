#include "og_ipushbutton.h"

using namespace og::ui;

bool IPushButton::hitButton(const QPoint & pos) const
{
    return rect().contains(pos);
}

