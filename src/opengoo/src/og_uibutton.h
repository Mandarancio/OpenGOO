#ifndef OG_UIBUTTON_H
#define OG_UIBUTTON_H

#include <QRect>
#include <QString>

struct WOGButton;

class QImage;
class QPainter;
class QMouseEvent;

class OGUIButton : public QRect
{
        QString onclick_;
        QString text_;

        QImage* pImg_;
        QImage* pUpImg_;
        QImage* pOverImg_;
        WOGButton* pConfig_;

        QString _GetText(QString &str);

    public:
        OGUIButton(WOGButton* config);
        ~OGUIButton();

        QString onclick() const { return onclick_; }

        void SetUpImage(const QString &path);
        void SetOverImage(const QString &path);

        virtual void Paint(QPainter* painter);

        // Events
        void MouseDown(QMouseEvent* ev);
        void MouseMove(QMouseEvent* ev);
        void Leave(QMouseEvent* ev);
};

#endif // OG_UIBUTTON_H
