#pragma once

#include "SoundEngine/music.h"
#include "SoundEngine/sound.h"
#include "imagesource.h"

class QString;
struct WOGBall;

namespace og
{
struct IResourceManager
{
    virtual ~IResourceManager()
    {
    }

    virtual bool ParseResourceFile(const QString& a_filename) = 0;

    virtual const WOGBall* GetBallByType(const QString& a_type) = 0;

    virtual ImageSourceSPtr GetImageSourceById(const QString& a_id) = 0;

    virtual MusicSPtr GetMusic(const QString& a_id) = 0;

    virtual SoundSPtr GetSound(const QString& a_id) = 0;

    virtual const og::audio::SoundSource* GetSoundSource(const QString& a_id) = 0;

    virtual void ClearAllSoundSources() = 0;
};
}

