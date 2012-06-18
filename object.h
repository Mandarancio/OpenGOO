#ifndef OBJECT_H
#define OBJECT_H

#include <QObject>

class QPainter;
class QRect;

class b2Body;


class Object : public QObject
{
    Q_OBJECT

public:
    Object(QObject* parent = 0) :
        QObject(parent)
    {
    }
    
    b2Body* getBody()
    {
        return body;
    }
    
    virtual QRect boundingRect() = 0;

protected:
    b2Body* body;

signals:

public slots:
    virtual void paint(QPainter& p)=0;
};

#endif // OBJECT_H
