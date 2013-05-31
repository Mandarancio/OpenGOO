#ifndef OG_WIDGETINTERFACE_H
#define OG_WIDGETINTERFACE_H

#include "og_iwindow.h"

namespace og
{
    class OGGame;
    class OGWidget;

    class WidgetInterface : public IWindow
    {
        public:
            WidgetInterface(OGGame* game);

        private:
            WidgetInterface(const WidgetInterface&);
            WidgetInterface& operator=(const WidgetInterface&);

            void _setGeometry(int xpos, int ypos, int width, int height);
            void _show();
            void _showFullScreen();
            void _grabKeyboard();
            void _releaseKeyboard();

            void _setActive(bool active);

            void _addUI(ui::IUI *ui);
            void _removeUI(ui::IUI *ui);

            void _close();

            std::unique_ptr<OGWidget> _pWidget;
    };

} // namespace og

#endif // OG_WIDGETINTERFACE_H
