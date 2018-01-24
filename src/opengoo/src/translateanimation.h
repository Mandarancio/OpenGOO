#pragma once

#include "animation.h"

class TranslateAnimation : public Animation
{
    enum
    {
        e_x,
        e_y
    };

public:
    TranslateAnimation(og::Entity* aEntity,
                       int aDuration,
                       float aStartX,
                       float aStartY,
                       float aEndX,
                       float aEndY)
            : Animation(aEntity, aDuration)
    {
        mXSpeed = (aEndX - aStartX) / mDuration;
        mYSpeed = (aEndY - aStartY) / mDuration;
        mStartPosition = mEntity->GetPosition();
    }

private:
    void Restart()
    {
        mEntity->SetPosition(mStartPosition);
        Start();
    }

    void Start()
    {
        mPosition = mEntity->GetPosition();
        mX = 0.0f;
        mY = 0.0f;
        mRemainingTime = mDuration;
        mIsActive = mRemainingTime > 0;
    }

    void Update(int aDt)
    {
        if (IsActive())
        {
            mEntity->SetPosition(QVector2D(mPosition.x() + mX, mPosition.y() + mY));
            mX += mXSpeed * aDt;
            mY += mYSpeed * aDt;

            mRemainingTime -= aDt;
            if (!(mRemainingTime > 0))
            {
                mIsActive = false;
            }
        }
    }

private:
    QVector2D mStartPosition;
    QVector2D mPosition;
    float mXSpeed;
    float mYSpeed;
    float mX;
    float mY;
};
