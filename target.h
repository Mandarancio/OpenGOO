#ifndef TARGET_H
#define TARGET_H

#include <QObject>
#include <QPoint>
#include <QPainter>
#include <Box2D/Box2D.h>

class Target : public QObject
{
    Q_OBJECT
public:
    explicit Target(QPoint position,int height,b2World * world,QObject *parent = 0);
private:
    QPoint position;
    int h;
    b2Body* body;
signals:

public slots:
    void paint(QPainter &p);
};

#endif // TARGET_H
