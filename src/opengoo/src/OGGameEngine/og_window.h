#ifndef OG_WINDOW_H
#define OG_WINDOW_H

#include <QWindow>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>

class OGWindow : public QWindow
{
    Q_OBJECT

public:
    explicit OGWindow(QWindow* parent = 0);
    virtual ~OGWindow();

    QPainter* getPainter();
    void swapBuffers();
    void render();

public slots:

protected:
    void keyReleaseEvent(QKeyEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void exposeEvent(QExposeEvent* event);
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private:
    bool isGameStarted_;
    bool isVideoInit_;

    QOpenGLContext* context_;
    QOpenGLFunctions* gFunctions_;
    QOpenGLPaintDevice* paintDevice_;

    bool initOpenGL_();    
};

#endif // OG_WINDOW_H
