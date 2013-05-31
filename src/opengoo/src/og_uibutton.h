#ifndef OG_UIBUTTON_H
#define OG_UIBUTTON_H

#include <QImage>
#include <QRect>
#include <QString>

struct WOGButton;

struct OGSprite;

class QImage;
class QPainter;
class QMouseEvent;

namespace og
{
namespace ui
{
class OGUIButton : public QRect
{
    public:
    struct Sprites
        {
            Sprites(OGSprite* upimage=0, OGSprite* overimage=0)
                : upImage(upimage)
                , overImage(overimage) {}

            OGSprite* upImage;
            OGSprite* overImage;
        };

        OGUIButton();
        OGUIButton(WOGButton* config);
        ~OGUIButton();

        QString onclick() const { return _GetOnClick(); }

        bool isOver();

        void SetUpImage(const QString &path);
        void SetOverImage(const QString &path);

        void AddSprites(const OGUIButton::Sprites &spritelist);
        void SetOnClick(const QString &onclick);

        virtual void Paint(QPainter* painter);

        // Events
        void MouseDown(QMouseEvent* ev);
        void MouseMove(QMouseEvent* ev);
        void Leave(QMouseEvent* ev);               

    private:
        struct OGUIButtonImpl* pImpl_;

        const QString &_GetOnClick() const;
        QString _GetText(QString &str);

        void _SetUpImage(const QString &path);
        void _SetOverImage(const QString &path);
};
} // namespace ui
} // namespace og

#endif // OG_UIBUTTON_H
