#pragma once

#include "og_ui.h"

#include <QObject>

class QColor;
class QPixmap;
class QPoint;
class QString;

namespace og
{
namespace ui
{
class IPushButton : public QObject, public UI
{
    Q_OBJECT

public:
    virtual ~IPushButton() {}

    void setText(const QString &text) { _setText(text); }

    void setFont(const QFont &font) { _setFont(font); }
    void setFontColor(const QColor & color) { _setFontColor(color); }

    void setUpImage(QPixmap* pm) { _setUpImage(pm); }
    void setOverImage(QPixmap* pm) { _setOverImage(pm); }

signals:
    void pressed();
    void released();
    void over();
    void leave();

protected:
    virtual bool hitButton(const QPoint &pos) const;

private:
    virtual void _setText(const QString &text) = 0;

    virtual void _setFont(const QFont &font) = 0;
    virtual void _setFontColor(const QColor &color) = 0;

    virtual void _setUpImage(QPixmap* pm) = 0;
    virtual void _setOverImage(QPixmap* pm) = 0;
};

typedef std::unique_ptr<IPushButton> IPushButtonUPtr;

} // ns:ui
} // ns:og
