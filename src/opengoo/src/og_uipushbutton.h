#ifndef OG_UIPUSHBUTTON_H
#define OG_UIPUSHBUTTON_H

#include "og_uiwindow.h"

class QString;

class OGUIPushButton : public OGUIWindow
{
public:
    OGUIPushButton();
    ~OGUIPushButton();

    void SetText(const QString &text);
    void SetFontColor(int r, int g, int b, int a = 255);
    void SetFontColor(const QColor & color);

private:
    struct OGUIPushButtonImpl* pImpl_;

    void _Paint(QPainter *painter);
};

#endif // OG_UIPUSHBUTTON_H
