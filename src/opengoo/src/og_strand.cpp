#include "og_strand.h"
#include <OGPhysicsEngine>
#include "og_userdata.h"
#include <QPainter>
#include <QVector2D>

#include "physics.h"

using namespace og;

OGStrand::~OGStrand()
{
    PE->DestroyJoint(strand_);

    b1_->ReleaseStrand();
    b2_->ReleaseStrand();
}

OGStrand::OGStrand(OGBall* gb1, OGBall* gb2, int id)
    : b1_(gb1), b2_(gb2), id_(id)
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

            strand_ = PhysicsFactory::createJoint(b1_, b2_, data);
            b1_->body->SetFixedRotation(true);
            b2_->body->SetFixedRotation(true);
            b1_->AddStrand();
            b2_->AddStrand();
        }
        else if (type_ == "rope") {}

        else if (type_ == "rigid") {}
    }
}

void OGStrand::Paint(QPainter* painter, bool debug)
{
    Q_UNUSED(debug)

    int x1, y1, x2, y2;

    const qreal K = 10.0;

    if (b1_ && b2_)
    {
        x1 = b1_->GetX()*K;
        y1 = b1_->GetY()*K*(-1.0);
        x2 = b2_->GetX()*K;
        y2 = b2_->GetY()*K*(-1.0);

        QPen pen(Qt::yellow,  2.0);
        painter->save();        
        painter->setPen(pen);
        painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
        painter->restore();
    }
}

float OGStrand::GetLenghth()
{
    QVector2D v1 = b1_->GetPosition();
    QVector2D v2 = b2_->GetPosition();
    float l = (v1 - v2).length();

    return l;
}
