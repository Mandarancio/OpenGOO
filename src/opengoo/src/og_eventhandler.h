#ifndef OG_EVENTHANDLER_H
#define OG_EVENTHANDLER_H

#include "og_event.h"

template<OGEvent::Type T>
struct OGEventHandler
{
    static void Event() { }
};

#endif // OG_EVENTHANDLER_H
