#include "sound.h"

namespace og
{
namespace audio
{
SoundSource::SoundSource(const std::string& a_filename)
{
    if (!m_buffer.loadFromFile(a_filename))
    {
        throw std::runtime_error("Could not load a sound");
    }
}

Sound::Sound(const SoundSource& a_source)
    : m_sound(a_source.m_buffer)
{
}

void Sound::Play()
{
    m_sound.play();
}

SoundSourceList::SoundSourceList()
    : m_currentBuffer(0)
{
}

void SoundSourceList::Add(const SoundSource* a_source)
{
    m_soundSources.push_back(a_source);
}

void SoundSourceList::Play()
{
    auto src = m_soundSources[m_currentBuffer];
    m_sound.setBuffer(src->m_buffer);
    m_sound.play();
    ++m_currentBuffer;
    m_currentBuffer %= m_soundSources.size();
}
}
}
