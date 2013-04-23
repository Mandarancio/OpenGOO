#ifndef WOG_POBJECT_H
#define WOG_POBJECT_H

#include <QString>

// The basic class of World of Goo physical objects

struct WOGPObject
{
    QString id;
    bool dynamic;
    QString tag;
    QString material;

    virtual ~WOGPObject() {}
};

#endif // WOG_POBJECT_H
