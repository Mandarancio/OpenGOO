#pragma once

#include <array>
#include <vector>
#include <cassert>

#include "og_sprite.h"

class MultiSprite : public og::Graphic
{
public:
    MultiSprite(bool aFixed = true)
        : mIsFixed(aFixed)
    {
    }

    void AddSprite(OGSpritePtr&& aSpr, unsigned int aDepth = 0)
    {
        assert(aDepth < mRender.size());
        mRender[aDepth].push_back(aSpr);
    }

    float GetAngle() const
    {
        return  0;
    }

    void SetAngle(float)
    {
    }

    void SetScaleX(float)
    {
    }

    void SetScaleY(float)
    {
    }

    float GetScaleX() const
    {
        return 1;
    }

    float GetScaleY() const
    {
        return 1;
    }

private:
    void Render(QPainter& aPainter, const QVector2D& aPos)
    {
        for (size_t i = 0; i < mRender.size(); ++i)
        {
            for (auto it = mRender[i].begin(); it != mRender[i].end(); ++it)
            {
                auto& spr = *it;
                const auto& pos = mIsFixed ? spr->GetPosition() : aPos;
                spr->Render(aPainter, pos);
            }
        }
    }

    void Render(QPainter& aPainter, float aX, float aY)
    {
        Render(aPainter, QVector2D(aX, aY));
    }

    void Update()
    {
    }

private:
    std::array<std::vector<OGSpritePtr>, 5> mRender;
    bool mIsFixed;
};

