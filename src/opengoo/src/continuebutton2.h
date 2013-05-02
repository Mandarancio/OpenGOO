#ifndef CONTINUEBUTTON2_H
#define CONTINUEBUTTON2_H

#include "og_uipushbutton.h"

class ContinueButton2 : public OGUIPushButton
{
public:
    ContinueButton2();
    ~ContinueButton2();

private:
    bool _MouseButtonDown(QMouseEvent *ev);
};

#endif // CONTINUEBUTTON2_H
