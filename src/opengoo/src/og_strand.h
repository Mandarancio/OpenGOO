#ifndef OG_STRAND_H
#define OG_STRAND_H

#include <OGPhysicsJoint>
#include "entities/og_ball.h"

class QPainter;

class OGStrand
{
    OGBall* b1_;
    OGBall* b2_;
    og::physics::Joint* strand_;
    QString type_;
    int id_;
    og::physics::PhysicsEngine& m_physicEngine;

public:
    OGStrand(og::physics::PhysicsEngine& a_physicEngine, OGBall* gb1, OGBall* gb2, int id);
    virtual ~OGStrand();

    int id() const { return id_; }
    OGBall* b1() const { return b1_; }
    OGBall* b2() const { return b2_; }

    float GetLenghth();

    void Paint(QPainter* painter, bool debug=false);
};

#endif // OG_STRAND_H
