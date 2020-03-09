#pragma once

#include <memory>

#include <QStringList>

#include <SFML/Audio.hpp>
#include "audio.h"

namespace og
{
namespace audio
{
class SoundSourceList;

class SoundSource
{
    friend class Sound;
    friend class SoundSourceList;

public:
    SoundSource(const std::string& a_filename);

private:
    sf::SoundBuffer m_buffer;
};

class Sound : public Audio
{
public:
    Sound(const SoundSource& a_source);

    void Play();

    void SetLoop(bool aLoop);

private:
    sf::Sound m_sound;
};

class SoundSourceList : public Audio
{
public:
    SoundSourceList();

    void Play();

    void Add(const SoundSource* a_source);

private:
    std::vector<const SoundSource*> m_soundSources;
    size_t m_currentBuffer;
    sf::Sound m_sound;
};

class SoundUtil
{
public:
    static std::streambuf* SetStreamBuffer(std::streambuf *a_buf)
    {
        return sf::err().rdbuf(a_buf);
    }
};
}
}

typedef std::shared_ptr<og::audio::Sound> SoundSPtr ;
