#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QColor>
#include <QList>
#include <QObject>
#include <QPair>
#include <QPolygon>
#include <QScopedPointer>
class QPainter;

class BackGround : public QObject
{
    Q_OBJECT
    class ImageCache;

public:
    BackGround(int level = 0, QObject* parent = 0);
    ~BackGround();
    
    void addPolygon(const QPolygon& polygon, const QColor& color);
    
    int getLevel() const
    {
        return level;
    }
    
    void setDelta(float delta)
    {
        this->delta = delta;
    }

private:
    int level;
    float delta; //!< difference of the translation with the main objects
    QPoint translate;
    
    QScopedPointer<ImageCache> img_cache;
    QList < QPair<QPolygon, QColor> > polygons;

signals:

public slots:
    void setTranslate(QPoint p)
    {
        translate = QPoint(-p.x()*delta, -p.y()*delta);
    }
    
    void paint(QPainter& p);
};

#endif // BACKGROUND_H
