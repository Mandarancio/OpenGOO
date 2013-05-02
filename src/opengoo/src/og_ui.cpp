#include "og_ui.h"
#include "og_uibutton.h"
#include "og_uiwindow.h"

#include <QList>
#include <QMouseEvent>

typedef QList<OGUIButton*> buttonList;
typedef QList<OGUIWindow*> windowList;

struct OGUIImpl
{
    buttonList buttons;
    windowList uiList;
    OGUIButton* pButton;
    OGUIWindow* pUI;
};

OGUI::OGUI()
{
    pImpl_ = new OGUIImpl;
    _Reset();
}

OGUI::~OGUI()
{
    buttonList* buttons = &pImpl_->buttons;

    while (!buttons->isEmpty()) { delete buttons->takeFirst(); }   
}

void OGUI::Paint(QPainter* p)
{
    Q_FOREACH(OGUIButton* btn, pImpl_->buttons)
    {
        btn->Paint(p);
    }

    Q_FOREACH(OGUIWindow* btn, pImpl_->uiList)
    {
        btn->Paint(p);
    }
}

void OGUI::AddButton(OGUIButton* btn) { pImpl_->buttons << btn; }
void OGUI::Add(OGUIWindow* ui) { pImpl_->uiList << ui; }

bool OGUI::_MouseDown(QMouseEvent *ev)
{
    Q_FOREACH(OGUIButton* btn, pImpl_->buttons)
    {
        if (btn->contains(ev->pos()))
        {
            btn->MouseDown(ev);

            return true;
        }
    }    

    Q_FOREACH(OGUIWindow* btn, pImpl_->uiList)
    {
        if (btn->MouseDown(ev))
        {
            return true;
        }
    }

    return false;
}

inline void OGUI::_Reset() { pImpl_->pButton = 0; }
inline void OGUI::_SetButton(OGUIButton *button) { pImpl_->pButton = button; }

void OGUI::_MouseMove(QMouseEvent *e)
{
    OGUIButton* btn = pImpl_->pButton;

    if (btn)
    {
        if (btn->contains(e->pos()))
        {
            btn->MouseMove(e);

            return;
        }
        else
        {
            btn->Leave(e);
            _Reset();
        }
    }

    Q_FOREACH(OGUIButton* btn, pImpl_->buttons)
    {
        if (btn->contains(e->pos()))
        {
            _SetButton(btn);
            btn->MouseMove(e);
            break;
        }
    }
}

void OGUI::_MouseEvent(QMouseEvent* e)
{
    switch(e->type())
    {
    case QMouseEvent::MouseButtonPress:
        _MouseDown(e);
        break;

    case QMouseEvent::MouseMove:
        _MouseMove(e);
        break;

    default:
        break;
    }
}
