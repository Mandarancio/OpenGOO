#ifndef OG_UIBUTTON_H
#define OG_UIBUTTON_H

#include "og_sprite.h"
#include "wog_scene.h"

#include <QPainter>
#include <QPointF>
#include <QSizeF>
#include <QMouseEvent>
#include <QWindow>

#include "og_states.h"
#include "wog_scene.h"

class OGUIButton : public QRect
{
    QString onclick_;
    QString text_;

    QImage* img_;
    QImage* upImg_;
    QImage* overImg_;
    WOGButton* config_;

    WOGButton* config() const { return config_; }

public:
    OGUIButton(WOGButton* config);
    ~OGUIButton();

    QString onclick() const { return onclick_; }

    void SetUpImage(const QString& path);
    void SetOverImage(const QString& path) { overImg_ = new QImage(path); }

    virtual void Paint(QPainter* p, float zoom=1.0f);

    // Events
    void MouseDown(QMouseEvent *e);
    void MouseMove(QMouseEvent *e);
    void Leave(QMouseEvent *e);
};

#endif // OG_UIBUTTON_H
