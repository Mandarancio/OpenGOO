#ifndef WOG_PIPE_H
#define WOG_PIPE_H

#include <QList>
#include <QPointF>
#include <QString>

struct WOGPipe
{
    QString id;
    QString type;
    float depth;
    QList<QPointF>* vertex;

    WOGPipe();
    ~WOGPipe();
};

#endif // WOG_PIPE_H
