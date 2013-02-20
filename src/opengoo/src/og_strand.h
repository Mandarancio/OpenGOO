#ifndef OG_STRAND_H
#define OG_STRAND_H

#include "og_physicsjoint.h"
#include "og_ball.h"

#include <QPainter>

class OGStrand
{
    OGBall* b1_;
    OGBall* b2_;
    OGPhysicsJoint* strand_;
    QString type_;
    int id_;

public:
    OGStrand(OGBall* gb1, OGBall* gb2, int id);
    virtual ~OGStrand();

    int id() const { return id_; }

    void Paint(QPainter* painter, bool debug=false);
};

#endif // OG_STRAND_H
