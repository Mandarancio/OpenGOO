#include "continuebutton2.h"
#include "opengoo.h"
#include "og_event.h"
#include "GameEngine/og_gameengine.h"

ContinueButton2::ContinueButton2()
{    
    SetPosition(20, 50);
    SetSize(120, 40);
    SetText("Continue");
    OGGameEngine::getEngine()->addWindow("WND_CONTINUE", this);
}

ContinueButton2::~ContinueButton2()
{
     OGGameEngine::getEngine()->RemoveWindow("WND_CONTINUE");
}

bool ContinueButton2::_MouseButtonDown(QMouseEvent *ev)
{
    Q_UNUSED(ev)

    OpenGOO::instance()->SendEvent(new OGEvent(OGEvent::CONTINUE));

    return true;
}
