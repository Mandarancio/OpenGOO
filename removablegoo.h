#ifndef REMOVABLEGOO_H
#define REMOVABLEGOO_H


#include "dynamicgoo.h"

class RemovableGoo : public DynamicGoo
{
    Q_OBJECT
public:
    explicit RemovableGoo(b2World * world,QPoint point=QPoint(0,0),int radius=15,QObject *parent = 0);
    bool isDragable();
signals:

public slots:
    void paintDebug(QPainter &p);

};

#endif // REMOVABLEGOO_H
