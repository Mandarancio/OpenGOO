#ifndef PROGRESSWINDOW_H
#define PROGRESSWINDOW_H

#include "og_uiwindow.h"

class ProgressWindow : public OGUIWindow
{
public:
    ProgressWindow(int width, int height);
    ~ProgressWindow();

    void SetBalls(int balls);

private:
    struct ProgressWindowImpl* pImpl_;

    ProgressWindow(const ProgressWindow&);
    ProgressWindow& operator= (const ProgressWindow&);
};

#endif // PROGRESSWINDOW_H
