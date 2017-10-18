#pragma once

namespace og
{
namespace audio
{
class Audio
{
public:
    virtual ~Audio()
    {
    }

    virtual void Play() = 0;
};
}
}

typedef std::shared_ptr<og::audio::Audio> AudioSPtr;
