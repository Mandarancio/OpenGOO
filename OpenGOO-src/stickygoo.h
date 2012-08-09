#ifndef STICKYGOO_H
#define STICKYGOO_H

#include "removablegoo.h"

class StickyGoo : public RemovableGoo
{
    Q_OBJECT
public:
    explicit StickyGoo(b2World*world,QPoint point=QPoint(0,0),int radius=15,QObject *parent = 0);

signals:

public slots:

};

#endif // STICKYGOO_H
