#include "music.h"

namespace og
{
namespace audio
{
Music::Music(const std::string& a_filename)
{
    if (!m_music.openFromFile(a_filename))
    {
        throw std::runtime_error("Could not load a music file");
    }
}

void Music::SetLoop(bool a_flag)
{
    m_music.setLoop(a_flag);
}

void Music::Play()
{
    m_music.play();
}
}
}