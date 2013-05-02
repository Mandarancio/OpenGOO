#include "continuebutton.h"
#include "opengoo.h"
#include "og_event.h"

bool ContinueButton::_MouseButtonDown(QMouseEvent *ev)
{
    Q_UNUSED(ev)

    OpenGOO::SendEvent(new OGEvent(OGEvent::BACKTO_ISLAND));

    return true;
}
