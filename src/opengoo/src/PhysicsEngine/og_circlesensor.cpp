#include "og_circlesensor.h"
#include "circle.h"

#include <OGPhysicsEngine>
#include <OGPCircle>

#include <QString>

using namespace og;

OGCircleSensor::OGCircleSensor(const QString &id, const Circle &circle)
    : OGSensor(id)
{
    pBody_ = PEngine::GetInstance()->CreateCircle(circle);
    pBody_->fixture->SetSensor(true);
}

OGCircleSensor::~OGCircleSensor()
{
    delete pBody_;
}

Fixture* OGCircleSensor::_GetFixture() const
{
    return pBody_->fixture;
}

QVector2D OGCircleSensor::_GetPosition() const
{
    return pBody_->GetPosition();
}

void OGCircleSensor::_SetCategory(unsigned short category)
{
    b2Filter filter = pBody_->fixture->GetFilterData();
    filter.categoryBits = category;
    pBody_->fixture->SetFilterData(filter);
}

void OGCircleSensor::_SetMask(unsigned short mask)
{
    b2Filter filter = pBody_->fixture->GetFilterData();
    filter.maskBits = mask;
    pBody_->fixture->SetFilterData(filter);
}

void OGCircleSensor::_SetFilter(const OGSensorFilter &filter)
{
    b2Filter f;
    f.categoryBits = filter.category;
    f.maskBits = filter.mask;
    pBody_->fixture->SetFilterData(f);
}
