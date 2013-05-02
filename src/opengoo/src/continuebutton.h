#ifndef CONTINUEBUTTON_H
#define CONTINUEBUTTON_H

#include "og_uipushbutton.h"

class ContinueButton : public OGUIPushButton
{
public:
    bool _MouseButtonDown(QMouseEvent *ev);
};

#endif // CONTINUEBUTTON_H
