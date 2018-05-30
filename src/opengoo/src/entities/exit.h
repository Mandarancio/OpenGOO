#pragma once

#include "GameEngine/entity.h"

struct ExitEventListener;
class Ball;

class Exit : public og::Entity
{
public:
    Exit(std::unique_ptr<og::PhysicsBody> aPhysicsBody);

    void SetListener(ExitEventListener* aListener)
    {
        mListener = aListener;
    }

    bool IsClosed() const
    {
        return mIsClosed;
    }

    bool IsOpen() const
    {
        return !mIsClosed;
    }

private:
    void Update();

    void LastUpdate();

    void CollideWith(Ball* aBall);

private:
    ExitEventListener* mListener;
    bool mIsClosed;
};
