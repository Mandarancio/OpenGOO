#pragma once

#include "og_sprite.h"

class MultiSprite : public og::Graphic
{
public:
    void AddSprite(OGSpritePtr&& aSpr)
    {
        mSprites.push_back(aSpr);
    }

private:
    void Render(QPainter& aPainter, const QVector2D&)
    {
        for (auto& spr : mSprites)
        {
            spr->Render(aPainter, spr->GetPosition());
        }
    }

    void Update()
    {
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
    std::vector<OGSpritePtr> mSprites;
};

