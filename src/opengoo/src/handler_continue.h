#ifndef HANDLER_CONTINUE_H
#define HANDLER_CONTINUE_H

#include "opengoo.h"

template<>
struct OGEventHandler<OGEvent::CONTINUE>
{
    static void Event()
    {
        OpenGOO::instance()->ShowProgress();
    }
};

#endif // HANDLER_CONTINUE_H
