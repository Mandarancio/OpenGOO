#pragma once

class QMouseEvent;

namespace og
{
struct MouseListener
{
    virtual ~MouseListener()
    {
    }

    virtual void OnMouseDown(QMouseEvent* aEv) = 0;

    virtual void OnMouseUp(QMouseEvent* aEv) = 0;

    virtual void OnMouseMove(QMouseEvent* aEv) = 0;
};
}
