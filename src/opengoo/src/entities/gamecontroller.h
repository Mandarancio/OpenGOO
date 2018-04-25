#pragma once

#include "GameEngine/entity.h"
#include "SoundEngine/sound.h"

class Animator;

class GameController : public og::Entity
{
public:
    GameController();
    ~GameController();

    void SetMusic(const QString& aMusic)
    {
        mMusic = aMusic;
    }

    void AddLoopSound(SoundSPtr aSound)
    {
        mLoopSounds.push_back(aSound);
    }

private:
    void Update();

    void Added();

    void Removed();

    void Render(QPainter& a_painter);

private:
    QString mMusic;
    std::list<SoundSPtr> mLoopSounds;
};
