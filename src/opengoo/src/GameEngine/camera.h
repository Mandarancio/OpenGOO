#pragma once

#include "OGLib/point.h"
#include "OGLib/size.h"

#include "entity.h"
#include "input.h"

#include "animator.h"

namespace og
{
static const float OFFSET = 50.0f;
static const int DEFAULT_H_SPEED = 4;
static const int DEFAULT_V_SPEED = 4;

class Camera : public og::Entity
{
    oglib::SizeI mSize;
    float mZoom;
    oglib::SizeF mScrollSpeed;
    std::shared_ptr<Animator> mAnimator;

public:
    Camera()
        : Entity(0.0f, 0.0f)
        , mZoom(1.0f)
        , mScrollSpeed(DEFAULT_H_SPEED, DEFAULT_V_SPEED)
    {
    }

    void SetSize(int aWidth, int aHeight)
    {
        mSize.Set(aWidth, aHeight);
    }

    int GetWidth() const
    {
        return mSize.width();
    }

    int GetHeight() const
    {
        return mSize.height();
    }

    float GetScaledWidth() const
    {
        return mSize.width() * mZoom;
    }

    float GetScaledHeight() const
    {
        return mSize.height() * mZoom;
    }

    void SetZoom(float aZoom)
    {
        Q_ASSERT_X(aZoom != 0.0f, "SetZoom", "aZoom equal to zero");
        mZoom = aZoom;
    }

    float GetZoom() const
    {
        return mZoom;
    }

    const oglib::SizeF& GetScrollSpeed() const
    {
        return mScrollSpeed;
    }

    void SetScrollSpeed(float aWidth, float aHeight)
    {
        mScrollSpeed.Set(aWidth, aHeight);
    }

    void SetAnimator(std::shared_ptr<Animator> aAnimator)
    {
        mAnimator = aAnimator;
    }

    void Update()
    {
        if (mAnimator && mAnimator->IsActive())
        {
            mAnimator->Update();
        }
        else
        {
            Scroll();
        }
    }

private:
    void Scroll()
    {
        auto pos = og::MouseInput::GetPosition();
        auto speedX = 0.0f;
        if (pos.x() <= OFFSET)
        {
            speedX = -GetScrollSpeed().width();
        }
        else if (pos.x() >= mSize.width() - OFFSET)
        {
            speedX = GetScrollSpeed().width();
        }

        auto speedY = 0.0f;
        if (pos.y() <= OFFSET)
        {
            speedY = GetScrollSpeed().height();
        }
        else if (pos.y() >= mSize.height() - OFFSET)
        {
            speedY = -GetScrollSpeed().height();
        }

        SetPosition(GetPosition().x() + speedX, GetPosition().y() + speedY);
    }
};
} // ns:og
