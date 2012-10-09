#ifndef REMOVABLEGOO_H
#define REMOVABLEGOO_H


#include "dynamicgoo.h"

class RemovableGoo : public DynamicGoo
{
    Q_OBJECT
public:
    explicit RemovableGoo(b2World * world,QPoint point=QPoint(0,0),int radius=15,QObject *parent = 0);
    bool isDragable() {//The only difference with dynamic goo is that the joint status is ignored.
        return !isSleeping();
    }

signals:

public slots:

};

#endif // REMOVABLEGOO_H
