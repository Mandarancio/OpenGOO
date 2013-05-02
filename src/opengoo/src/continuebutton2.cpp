#include "continuebutton2.h"
#include "opengoo.h"
#include "og_event.h"
#include "GameEngine/og_gameengine.h"

ContinueButton2::ContinueButton2()
{
    int w = 120;
    int h = 40;

    OGGameEngine* ge = OGGameEngine::getEngine();

    int x = ge->getWidth() - w - 50;
    int y = 50;

    SetSize(w, h);
    SetPosition(x, y);
    SetText("Continue");
    SetBG(0, 0, 0, 32);
    SetFontColor(Qt::white);

    ge->addWindow("WND_CONTINUE", this);
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
