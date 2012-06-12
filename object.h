#ifndef OBJECT_H
#define OBJECT_H

#include <QObject>
#include <Box2D/Box2D.h>
#include <QPainter>

class Object : public QObject
{
    Q_OBJECT
public:
    explicit Object(QObject *parent = 0);
    b2Body* getBody();
    virtual QRect boundingRect()=0;
protected:
    b2Body * body;
signals:

public slots:
    virtual void paint(QPainter &p)=0;
};

#endif // OBJECT_H
