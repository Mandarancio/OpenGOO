#pragma once

#include <QGLWidget>
#include <QHash>
#include <QTimer>

#include "og_iui.h"


class QString;

namespace og
{
    class OGGame;

    class OGWidget : public QGLWidget
    {
            Q_OBJECT

        public:
            explicit OGWidget(OGGame* game);

            void setActive(bool active);

            void addUI(ui::IUI* ui);
            void removeUI(ui::IUI* ui);

            void setBackgroundColor(const QColor& col, bool show);

        protected:
            void keyReleaseEvent(QKeyEvent* ev);
            void keyPressEvent(QKeyEvent* ev);

            void showEvent(QShowEvent*);
            void resizeEvent(QResizeEvent*);
            void mousePressEvent(QMouseEvent* ev);
            void mouseReleaseEvent(QMouseEvent* ev);
            void mouseMoveEvent(QMouseEvent* ev);
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
    };

} // namespace og
