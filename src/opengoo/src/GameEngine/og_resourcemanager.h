#pragma once

#include <fstream>

#include <QHash>
#include <QString>

#include "wog_ball.h"
#include "imagesource.h"
#include "SoundEngine/music.h"
#include "SoundEngine/sound.h"

class WOGResources;

namespace og
{
class OGResourceManager
{
    template <typename T>
    struct AudioEntry
    {
        QString id;
        T audio;
    };

    typedef AudioEntry<MusicSPtr> MusicEntry;
    typedef AudioEntry<SoundSPtr> SoundEntry;

public:
    typedef std::unique_ptr<WOGBall> WOGBallPtr;
    typedef std::shared_ptr<WOGBall> WOGBallSPtr;
    typedef std::shared_ptr<ImageSource> ImageSourceSPtr;
    typedef std::shared_ptr<WOGResources> WOGResourcesSPtr;

public:
    OGResourceManager();
    ~OGResourceManager();

    bool ParseResourceFile(const QString& a_filename);

    WOGBall* GetBallByType(const QString& a_type);

    ImageSourceSPtr GetImageSourceById(const QString& a_id);

    MusicSPtr GetMusic(const QString& a_id);

    SoundSPtr GetSound(const QString& a_id);

    const og::audio::SoundSource* GetSoundSource(const QString& a_id);

    void ClearAllSoundSources()
    {
        m_soundSources.clear();
    }

private:
    template<typename T>
    bool Load(T& a_ball, const QString& a_filename);

    og::audio::SoundSource* AddSoundSource(const QString a_id);

private:
    QHash<QString, WOGResourcesSPtr> m_resources;
    QHash<QString, WOGBallSPtr> m_balls;
    QHash<QString, ImageSourceSPtr> m_imageSources;
    QHash<QString, og::audio::SoundSource> m_soundSources;
    MusicEntry m_Music;
    std::ofstream m_file;
    std::streambuf* m_previous;
};
}
