#ifndef OG_FPSCOUNTER_H
#define OG_FPSCOUNTER_H

#include "og_uiwindow.h"

class OGFPSCounter : public OGUIWindow
{
public:
    OGFPSCounter();
    ~OGFPSCounter();

    void Reset();
    void Update(int time);

private:
    struct OGFPSCounterImpl* pImpl_;
};

#endif // OG_FPSCOUNTER_H
