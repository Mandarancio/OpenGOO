#ifndef WOG_EXIT_H
#define WOG_EXIT_H

#include <QString>
#include <QPointF>

struct WOGLevelExit
{
    QString id;
    QPointF pos;
    float radius;
    QString filter;
};

#endif // WOG_EXIT_H
