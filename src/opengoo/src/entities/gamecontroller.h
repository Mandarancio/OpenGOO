#pragma once

#include "GameEngine/entity.h"

class Animator;

class GameController : public og::Entity
{
    QString mMusic;

    void Update();

    void Added();

    void Removed();

    void Render(QPainter& a_painter);

public:
    GameController();
    ~GameController();

    void SetMusic(const QString& aMusic)
    {
        mMusic = aMusic;
    }
};
