#ifndef OG_STATICBODY_H
#define OG_STATICBODY_H

#include <QString>

struct OGPhysicsBody;

struct OGStaticBody
{
    OGPhysicsBody* body;
    QString tag;

    ~OGStaticBody();
};

#endif // OG_STATICBODY_H
