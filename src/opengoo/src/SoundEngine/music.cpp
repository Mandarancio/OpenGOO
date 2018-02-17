#include "music.h"

namespace og
{
namespace audio
{
Music::Music(const std::string& a_filename)
{
    if (!OpenFile(a_filename))
    {
        throw std::runtime_error("Could not load a music file");
    }
}

void Music::SetLoop(bool a_flag)
{
    m_music.setLoop(a_flag);
}

void Music::Play(bool aLoop)
{
    m_music.setLoop(aLoop);
    m_music.play();
}

void Music::Stop()
{
    m_music.stop();
}

bool Music::OpenFile(const std::string& a_filename)
{
    return m_music.openFromFile(a_filename);
}
}
}
