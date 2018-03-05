#pragma once

#include <QVector2D>

#include "animation.h"

class TranslateAnimation : public Animation
{
    enum
    {
        e_x,
        e_y
    };

public:
    TranslateAnimation(og::Entity* aEntity, int aDuration, const QPointF& aStart, const QPointF& aEnd)
            : Animation(aEntity, aDuration)
            , mSpeed((aEnd - aStart) / mDuration)
    {
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
        mDelta = QVector2D();
        mRemainingTime = mDuration;
        mIsActive = mRemainingTime > 0;
    }

    void Update(int aDt)
    {
        if (IsActive())
        {
            mEntity->SetPosition(mPosition + mDelta);
            mDelta += mSpeed * aDt;
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
    QVector2D mSpeed;
    QVector2D mDelta;
};
