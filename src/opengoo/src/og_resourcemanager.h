#pragma once

#include <QHash>
#include <QString>

#include "GameEngine/iresourcemanager.h"

class WOGResources;

class OGResourceManager : public og::IResourceManager
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
    typedef std::shared_ptr<WOGResources> WOGResourcesSPtr;

public:
    bool ParseResourceFile(const QString& a_filename);

    const WOGBall* GetBallByType(const QString& a_type);

    og::ImageSourceSPtr GetImageSourceById(const QString& a_id);

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

    og::ImageSourceSPtr CreateImageSource(const QString& a_filename);

private:
    QHash<QString, WOGResourcesSPtr> m_resources;
    QHash<QString, WOGBallSPtr> m_balls;
    QHash<QString, og::ImageSourceSPtr> m_imageSources;
    QHash<QString, og::audio::SoundSource> m_soundSources;
    MusicEntry m_Music;
};
