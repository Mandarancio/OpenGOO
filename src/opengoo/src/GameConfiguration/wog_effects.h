#ifndef WOG_EFFECTS_H
#define WOG_EFFECTS_H

#include <QDebug>

struct WOGEffects
{
    ~WOGEffects()
    {
        qDebug("WOGEffects: End");
    }
};

#endif // WOG_EFFECTS_H
