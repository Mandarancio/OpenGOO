#ifndef OG_UIFRAME_H
#define OG_UIFRAME_H

#include "og_ui.h"

class QColor;

namespace og {
namespace ui {
class Frame : public UI
{
public:
    Frame();
    ~Frame();

    void setBgColor(const QColor &color);

protected:
    void _Paint(QPainter* painter);

private:
    Frame(const Frame&);
    Frame& operator=(const Frame&);

    struct Impl;
    std::unique_ptr<Impl> _pImpl;
};

} // namespace ui

} // namespace og

#endif // OG_UIFRAME_H
