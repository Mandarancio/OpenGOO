#pragma once

struct ExitEventListener
{
    virtual ~ExitEventListener()
    {
    }

    virtual void OnOpen() = 0;
    virtual void OnClosed() = 0;
};

