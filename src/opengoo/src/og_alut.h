#ifndef OG_ALUT
#define OG_ALUT

#include <AL/al.h>
#include <AL/alc.h>

#include <OGTypes>

class OGAlut
{
public:
    static bool Init();
    static bool Exit();

    static OGuint CreateBuffer();
    static OGuint CreateSource();

    static void DeleteBuffer(OGuint buffer)  { alDeleteBuffers(1, &buffer); }
    static void DeleteSource(OGuint source) { alDeleteSources(1, &source); }
    static bool AttachBuffer(OGuint source, OGuint buffer);

    static void Play(OGuint source) { alSourcePlay(source); }
    static void Pause(OGuint source) { alSourcePause(source); }
    static void Stop(OGuint source) { alSourceStop(source); }

    static void SetVolume(OGuint source, float volume);
    static void SetPitch(OGuint source, float pitch);
    static void SetPosition(OGuint source, float x, float y) ;
    static int GetState(OGuint source);

    static OGuint CreateBufferFromFile (const OGpchar filename);

private:
    static OGuint CreateBufferFromOgg(const OGpchar filename);
    static OGuint CreateBufferFromWav(const OGpchar filename);
};

#endif // OG_ALUT
