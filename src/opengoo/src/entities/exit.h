#pragma once

#include "GameEngine/entity.h"

struct ExitEventListener;
class Ball;

class Exit : public og::Entity
{
public:
    Exit(std::unique_ptr<og::PhysicsBody> aPhysicsBody);

    void AddListener(ExitEventListener* aListener)
    {
        mListener.push_back(aListener);
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
    std::list<ExitEventListener*> mListener;
    bool mIsClosed;
};
