#ifndef OG_WINDOW_H
#define OG_WINDOW_H

#include <QWindow>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

class OGWindow : public QWindow
{
    Q_OBJECT

public:
    explicit OGWindow(QWindow* parent=0);
    virtual ~OGWindow();

    void render();

protected:
    void keyReleaseEvent(QKeyEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void exposeEvent(QExposeEvent* event);
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private:
    QOpenGLContext* context_;
    QOpenGLPaintDevice* paintDevice_;

    bool initOpenGL_();
};

#endif // OG_WINDOW_H
