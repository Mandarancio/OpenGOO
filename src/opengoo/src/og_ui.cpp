#include "og_ui.h"

void OGUI::Paint(QPainter* p, float zoom)
{
    Q_FOREACH(OGUIButton * btn, buttons_)
    {
        btn->Paint(p, zoom);
    }
}

void OGUI::_MouseDown(QMouseEvent *e)
{    
    Q_FOREACH(OGUIButton* btn, buttons_)
    {
        if (btn->contains(e->pos()))
        {
            btn->MouseDown(e);

            break;
        }
    }
}

void OGUI::_MouseMove(QMouseEvent *e)
{
    if (btn_ != 0)
    {
        if (btn_->contains(e->pos()))
        {
            btn_->MouseMove(e);

            return;
        }
        else
        {
            btn_->Leave(e);
            btn_ = 0;
        }
    }

    Q_FOREACH(OGUIButton* btn, buttons_)
    {
        if (btn->contains(e->pos()))
        {
            btn_ = btn;
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
