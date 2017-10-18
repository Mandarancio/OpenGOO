#include "og_strand.h"
#include <OGPhysicsEngine>
#include "og_userdata.h"
#include <QPainter>
#include <QVector2D>

#include "physics.h"

using namespace og;

OGStrand::~OGStrand()
{
    m_physicEngine.DestroyJoint(strand_);

    b1_->ReleaseStrand();
    b2_->ReleaseStrand();
}

OGStrand::OGStrand(og::physics::PhysicsEngine& a_physicEngine, OGBall* gb1, OGBall* gb2, int id)
    : b1_(gb1)
    , b2_(gb2)
    , id_(id)
    , m_physicEngine(a_physicEngine)
{
    strand_ = 0;
    type_ = b1_->GetStrandType();

    if (type_ == b2_->GetStrandType())
    {
        if (type_ == "spring")
        {
            OGUserData* data = new OGUserData;
            data->type = OGUserData::STRAND;
            data->data = this;

            strand_ = PhysicsFactory::createJoint(a_physicEngine, b1_->GetBody(), b2_->GetBody(), data);
            b1_->GetBody()->body->SetFixedRotation(true);
            b2_->GetBody()->body->SetFixedRotation(true);
            b1_->AddStrand();
            b2_->AddStrand();
        }
        else if (type_ == "rope")
        {

        }
        else if (type_ == "rigid")
        {

        }
    }
}

void OGStrand::Paint(QPainter* painter, bool debug)
{
    Q_UNUSED(debug)

    int x1, y1, x2, y2;

    if (b1_ && b2_)
    {
        auto p1 = b1_->GetPhyPosition();
        auto p2 = b1_->GetPhyPosition();
        x1 = p1.x() * PhysicsFactory::MetersToPixels;
        y1 = p1.y() * PhysicsFactory::MetersToPixels * (-1.0);
        x2 = p2.x() * PhysicsFactory::MetersToPixels;
        y2 = p2.y() * PhysicsFactory::MetersToPixels * (-1.0);

        QPen pen(Qt::yellow,  2.0);
        painter->save();        
        painter->setPen(pen);
        painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
        painter->restore();
    }
}

float OGStrand::GetLenghth()
{
    QVector2D v = b1_->GetPhyPosition() - b2_->GetPhyPosition();

    return v.length();;
}
