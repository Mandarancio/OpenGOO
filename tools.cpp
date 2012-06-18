#include<QPolygon>
#include<QString>
#include "tools.h"

/*
        THANKS TO Slyshyk Oleksly for fixes
*/

QPolygon toPoly(const QList<QPoint>& ps)
{
    return QVector<QPoint>::fromList(ps);
}

QPolygon toPoly(const QList<QPoint>& ps, QPoint center)
{
    QPolygon poly;
    for (QList<QPoint>::const_iterator it = ps.begin(); it != ps.end(); ++it)
        poly << center + *it;
    return poly;
}



QString time2string(int time)
{
    int mm = time/60;
    int ss = time - mm*60;
    return mm ? QString::number(mm) + ":" + (ss>=10 ? "":"0")+QString::number(ss) : QString::number(ss);
}
