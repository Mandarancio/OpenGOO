#ifndef WOG_POBJECT_H
#define WOG_POBJECT_H

#include <QString>
#include <QDebug>

// The basic class of World of Goo physical objects

struct WOGPObject
{
    QString id;
    bool dynamic;
    QString tag;
    QString material;
};

#endif // WOG_POBJECT_H
