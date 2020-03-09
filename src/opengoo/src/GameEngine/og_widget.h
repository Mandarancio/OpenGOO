#pragma once

#include <array>
#include <memory>

#include <QGLWidget>
#include <QHash>
#include <QTimer>

#include "og_iui.h"
#include "input.h"

class QString;

namespace og
{
    class OGGame;

    class OGWidget : public QGLWidget
    {
            Q_OBJECT

        class Input;

        struct InputDeleter
        {
            void operator()(Input* ptr) const;
        };

        public:
            explicit OGWidget(OGGame* game);

            void setActive(bool active);

            void addUI(ui::IUI* ui);
            void removeUI(ui::IUI* ui);

            void setBackgroundColor(const QColor& col, bool show);

            bool IsButtonPressed(MouseInput::MouseButton aButton) const;

            QPoint GetMousePosition() const;

        protected:
            void keyReleaseEvent(QKeyEvent* ev);
            void keyPressEvent(QKeyEvent* ev);

            void showEvent(QShowEvent*);
            void resizeEvent(QResizeEvent*);
            void mousePressEvent(QMouseEvent* aEv);
            void mouseReleaseEvent(QMouseEvent* aEv);
            void mouseMoveEvent(QMouseEvent* aEv);
            void wheelEvent(QWheelEvent* ev);

            void paintEvent(QPaintEvent*);

            ui::UIList& uiList();
            const ui::UIList& uiList() const;

            // Returns the index of ui in the list
            int findUI(ui::IUI* ui);

            OGGame* getGame() { return _pGame; }

        private slots:
            void Update();

        private:
            OGGame* _pGame;
            QTimer _timer;
            ui::UIList _uiList;
            std::unique_ptr<Input, InputDeleter> mInput;
    };
} // namespace og
