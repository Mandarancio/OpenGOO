#ifndef OG_GAME_H
#define OG_GAME_H

class QKeyEvent;
class QMouseEvent;
class QPainter;
class QWheelEvent;

#include <QObject>

namespace og
{
    class OGGame : public QObject
    {
            virtual void _Start() = 0;
            virtual void _End() = 0;
            virtual void _Activate() = 0;
            virtual void _Deactivate() = 0;
            virtual void _Paint(QPainter* painter) = 0;
            virtual void _Cycle() = 0;
            virtual void _KeyDown(QKeyEvent* ev) = 0;
            virtual void _KeyUp(QKeyEvent* ev) = 0;
            virtual void _MouseButtonDown(QMouseEvent* ev) = 0;
            virtual void _MouseButtonUp(QMouseEvent* ev) = 0;
            virtual void _MouseMove(QMouseEvent* ev) = 0;
            virtual void _MouseWheel(QWheelEvent* ev) = 0;

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

} // namespace og

#endif // OG_GAME_H
