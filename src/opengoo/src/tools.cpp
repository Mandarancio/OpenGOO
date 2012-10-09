#include "tools.h"

#include <QPolygon>
#include <QString>

/*
        THANKS TO Slyshyk Oleksly for fixes
*/

QPolygon toPoly(const QList<QPoint>& ps, QPoint center)
{
    QPolygon poly;
    foreach(const QPoint& point, ps)
        poly << center + point;
    return poly;
}

QString time2string(int time)
{
    int mm = time/60;
    int ss = time - mm*60;
    return mm ? QString::number(mm) + ":" + (ss>=10 ? "":"0")+QString::number(ss) : QString::number(ss);
}
