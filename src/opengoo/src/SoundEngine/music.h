#pragma once

#include <SFML/Audio.hpp>

namespace og
{
namespace audio
{
    class Music
    {
    public:
        void SetLoop(bool a_flag);

        void Play(bool aLoop = false);

        void Stop();

        bool OpenFile(const std::string& a_filename);

        static std::shared_ptr<Music> Create(const std::string& a_filename)
        {
            return std::shared_ptr<Music>(new Music(a_filename));
        }

        static std::shared_ptr<Music> Create()
        {
            return std::shared_ptr<Music>(new Music);
        }


    private:
        Music()
        {
        }

        Music(const std::string& a_filename);
        Music(const Music&);
        Music(Music&&);
        Music& operator=(const Music&);
        Music& operator=(Music&&);

    private:
        sf::Music m_music;
    };
}
}

typedef std::shared_ptr<og::audio::Music> MusicSPtr;
