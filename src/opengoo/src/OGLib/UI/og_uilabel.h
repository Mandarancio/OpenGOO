#ifndef OG_UILABEL_H
#define OG_UILABEL_H

#include "og_uiframe.h"

class QColor;
class QFont;
class QString;

namespace og
{
namespace ui
{
class Label : public Frame
{
    public:
        Label();
        ~Label();

        void setFont(const QFont &font);
        void setFontColor(const QColor& color);
        void setText(const QString &text);

    private:
        Label(const Label&);
        Label& operator=(const Label&);

        struct Impl;
        std::unique_ptr<Impl> _pImpl;

        void _Paint(QPainter* painter);
};

} // namespace ui
} // namespace og

#endif // OG_UILABEL_H
