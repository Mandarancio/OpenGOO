#ifndef OG_UI_H
#define OG_UI_H

#include "wog_scene.h"

class OGUIButton;

class QMouseEvent;
class QPainter;

class OGUI
{
    public:
        OGUI();
        ~OGUI();
        void AddButton(OGUIButton* btn);
        void Paint(QPainter* p);

        friend class OpenGOO;

    private:
        struct OGUIImpl* pImpl_;

        void _MouseDown(QMouseEvent* e);
        void _MouseMove(QMouseEvent* e);
        void _MouseEvent(QMouseEvent* e);

        void _Reset();
        void _SetButton(OGUIButton* button);
};

#endif // OG_UI_H
