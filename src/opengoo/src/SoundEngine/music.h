#pragma once

#include <SFML/Audio.hpp>

namespace og
{
namespace audio
{
    class Music
    {
    public:
        Music(const std::string& a_filename);

        void Play();

        void SetLoop(bool a_flag);

    private:
        sf::Music m_music;
    };
}
}

typedef std::shared_ptr<og::audio::Music> MusicSPtr;
