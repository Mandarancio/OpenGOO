#ifndef OG_UIWINDOW_H
#define OG_UIWINDOW_H

class OGUI;

class QColor;
class QPainter;
class QPoint;
class QRect;
class QSize;

class QMouseEvent;

class OGUIWindow
{
    public:
        OGUIWindow(OGUIWindow* parent = 0);
        virtual ~OGUIWindow();

        void Paint(QPainter* painter);

        void AddUI(OGUI* ui);

        const QColor &GetBG() const;
        QRect GetGeometry() const;
        const OGUIWindow* GetParent() const;
        const QPoint &GetPosition() const;
        const QSize &GetSize() const;

        void SetBG(const QColor &color);
        void SetParent(OGUIWindow* parent);
        void SetPosition(const QPoint &pos);
        void SetPosition(int posx, int posy);
        void SetSize(const QSize &size);
        void SetSize(int width, int height);

        void MoveCenter(const QPoint &pos);
        void MoveCenter(int posx, int posy);
        void Show();
        void Hide();

        bool isShow();

        bool MouseDown(QMouseEvent *ev);
        bool MouseButtonUp(QMouseEvent* ev);
        bool MouseMove(QMouseEvent *ev);

    protected:
        bool TestPoint(const QPoint &pos);
        QPoint GetAbsPosition();

    private:
        struct OGUIWindowImpl* pImpl_;       

        virtual void _Paint(QPainter* painter);

        virtual bool _MouseButtonDown(QMouseEvent* ev);
        virtual bool _MouseButtonUp(QMouseEvent* ev);
        virtual bool _MouseMove(QMouseEvent* ev);
};

#endif // OG_UIWINDOW_H
