#pragma once

class QKeyEvent;
class QMouseEvent;
class QPainter;
class QWheelEvent;

#include <QObject>

namespace og
{
    class Camera;

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
            virtual void _MouseWheel(QWheelEvent* ev) = 0;

        public:
            virtual ~OGGame()
            {
            }

            void Start() { _Start(); }
            void End() { _End(); }
            void Activate() { _Activate(); }
            void Deactivate() { _Deactivate(); }
            void Paint(QPainter* painter) { _Paint(painter); }
            void Cycle() { _Cycle(); }
            void KeyDown(QKeyEvent* ev) { _KeyDown(ev); }
            void KeyUp(QKeyEvent* ev) { _KeyUp(ev); }
            virtual void MouseButtonDown(const QPoint& aPos) = 0;
            virtual void MouseButtonUp(const QPoint& aPos) = 0;
            virtual void MouseMove(const QPoint& aPos) = 0;
            void MouseWheel(QWheelEvent* ev) { _MouseWheel(ev); }
            virtual Camera* GetCamera() = 0;
    };

} // ns:og
