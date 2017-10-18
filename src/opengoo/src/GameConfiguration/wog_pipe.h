#pragma once

#include <QVector>
#include <QPointF>
#include <QString>

struct WOGPipe
{
    QString id;
    QString type;
    float depth;
    QVector<QPointF> vertex;
};
