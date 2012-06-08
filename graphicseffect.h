#ifndef GRAPHICSEFFECT_H
#define GRAPHICSEFFECT_H

#include <QGraphicsBlurEffect>


class GraphicsEffect : public QObject
{
    Q_OBJECT
public:
    explicit GraphicsEffect(QObject *parent = 0);
    void paint(QPainter &p,QImage &img);
    int getRadius();
    void setRadius(int radius);
private:
    int radius;
    QImage blurred(const QImage& image, const QRect& rect, int radius, bool alphaOnly = false);
signals:


public slots:

};

#endif // GRAPHICSEFFECT_H
