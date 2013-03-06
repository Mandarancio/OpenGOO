#ifndef OG_UI_H
#define OG_UI_H

#include "og_uibutton.h"
#include "wog_scene.h"
#include "wog_resources.h"

#include <QList>
#include <QMouseEvent>

class OGUI
{
    QList<OGUIButton*> buttons_;
    OGUIButton* btn_;

    void _MouseDown(QMouseEvent* e);
    void _MouseMove(QMouseEvent* e);
    void _MouseEvent(QMouseEvent* e);

    friend void mouseEven(OGUI* ui, QMouseEvent* ee);

public:
    void AddButton(OGUIButton* btn) { buttons_ << btn; }   
    void Paint(QPainter* p, float zoom);
};

#endif // OG_UI_H
