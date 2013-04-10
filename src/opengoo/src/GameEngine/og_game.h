#ifndef OG_GAME_H
#define OG_GAME_H

class QKeyEvent;
class QMouseEvent;
class QPainter;
class QWheelEvent;

class OGGame
{
    virtual void _Start() {}
    virtual void _End() {}
    virtual void _Activate() {}
    virtual void _Deactivate() {}
    virtual void _Paint(QPainter* painter) {}
    virtual void _Cycle() {}
    virtual void _KeyDown(QKeyEvent* ev) {}
    virtual void _KeyUp(QKeyEvent* ev) {}
    virtual void _MouseButtonDown(QMouseEvent* ev) {}
    virtual void _MouseButtonUp(QMouseEvent* ev) {}
    virtual void _MouseMove(QMouseEvent* ev) {}
    virtual void _MouseWheel(QWheelEvent* ev) {}

public:
    OGGame() {}
    virtual ~OGGame() {}

    void Start() { _Start(); }
    void End() { _End(); }
    void Activate() { _Activate(); }
    void Deactivate() { _Deactivate(); }
    void Paint(QPainter* painter) { _Paint(painter); }
    void Cycle() { _Cycle(); }
    void KeyDown(QKeyEvent* ev) { _KeyDown(ev); }
    void KeyUp(QKeyEvent* ev) { _KeyUp(ev); }
    void MouseButtonDown(QMouseEvent* ev) { _MouseButtonDown(ev); }
    void MouseButtonUp(QMouseEvent* ev) { _MouseButtonUp(ev); }
    void MouseMove(QMouseEvent* ev) { _MouseMove(ev); }
    void MouseWheel(QWheelEvent* ev) { _MouseWheel(ev); }
};

#endif // OG_GAME_H
