#pragma once

#include "GameEngine/entity.h"
#include "SoundEngine/sound.h"
#include "SoundEngine/music.h"

class Animator;

class GameController : public og::Entity
{
public:
    GameController();
    ~GameController();

    void SetMusic(og::audio::Music* aMusic)
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
    og::audio::Music* mMusic;
    std::list<SoundSPtr> mLoopSounds;
};
