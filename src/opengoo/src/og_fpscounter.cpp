#include "og_fpscounter.h"
#include "GameEngine/og_gameengine.h"
#include "og_uilabel.h"
#include "og_ui.h"

struct OGFPSCounterImpl
{
    int fps;
    int curFPS;
    int time;
    OGUILabel label;
    OGUI ui;
};

OGFPSCounter::OGFPSCounter()
{
    pImpl_ = new OGFPSCounterImpl;

    Reset();

    SetBG(Qt::transparent);

    OGUILabel* label = &pImpl_->label;

    label->SetParent(this);
    label->SetBG(Qt::transparent);
    label->SetFontColor(Qt::white);

    pImpl_->ui.Add(label);
    AddUI(&pImpl_->ui);

    OGGameEngine::getEngine()->addWindow("WND_FPS_COUNTER", this);
}

OGFPSCounter::~OGFPSCounter()
{
    OGGameEngine::getEngine()->RemoveWindow("WND_FPS_COUNTER");
    delete pImpl_;
}

void OGFPSCounter::Reset()
{
    pImpl_->time = 0;
    pImpl_->curFPS = 0;
    pImpl_->fps = 0;
}

void OGFPSCounter::Update(int time)
{
    pImpl_->curFPS++;
    pImpl_->time += time;

    if (pImpl_->time >= 1000)
    {
        pImpl_->fps = pImpl_->curFPS;
        pImpl_->curFPS = 0;
        pImpl_->time = 0;
        pImpl_->label.SetText(QString::number(pImpl_->fps));
    }    
}
