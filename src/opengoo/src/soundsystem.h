#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <vector>
#include <list>

#include <QObject>
#include <QPair>
#include <QPoint>
#include <QList>
#include <QThread>

#include <OGAlut>
#include <OGMPlayer>

/*!
 * The SoundSystem class manages the sound using OpenAL.
 */

struct OGShareBuffer
{   
    int type; // Type of share buffer
    int counter; // Number of share buffers of this type
    OGuint buffer; // Buffer which contain audio data

    OGShareBuffer(int soundType, OGuint shareBuffer)
        : type(soundType), counter(1), buffer(shareBuffer) {}

    OGShareBuffer(int soundType, int numBuffers, OGuint shareBuffer)
        : type(soundType), counter(numBuffers), buffer(shareBuffer) {}

};

struct OGSoundObject
{
    int id; // Id of sound object
    OGuint source; // Source with attached share buffer
    int type; // Type of the share buffer

    OGSoundObject(int idSoundObject, OGuint sourceId, int typeBuffer)
        : id(idSoundObject), source(sourceId), type(typeBuffer) {}
};

class SoundSystem : public QObject
{
        Q_OBJECT

public:
    static SoundSystem* GetInstance(void);
    static void DestroyInstance(void) { if (m_instance_) delete m_instance_; }

    enum typeSound{NONETYPE, SCREAM, BOING, POP, CAPTURED, DRAG};

    void Open(); // Initialize sound system
    void Close(); // Deinitialize sound system

    int Create(typeSound type); // Create sound object
    void Delete(int id);    // Delete sound object


    void SetVolume(int id, float volume); // Set volume of sound object
    void SetPitch(int id, float pitch); // Set ptich of sound object
    void SetPosition(int id, QPoint p); // Set position of sound object
    void SetCenter(QPoint p); // ??
    void Play(int id); // Play sound object
    void Stop(int id); // Stop sound object
    void Pause(int id); // Pause sound object

    int Status(int id); // Get status of sound object

    void OpenBGMusic(const OGpchar filename, bool loopmode=false, bool streammode=false);
    void CloseBGMusic();
    void PlayBGMusic();
    void StopBGMusic();
    void PauseBGMusic();

    bool IsFail() const { return isFailbit_; }
    bool IsOpen() const { return isOpen_; }

private:
    static SoundSystem* m_instance_;
    std::list<OGSoundObject> soundObjects_;
    std::list<OGShareBuffer> shareBuffers_;
    int soundSourceID_;
    QPoint center_;
    OGMPlayer* player_;

    bool isFailbit_;
    bool isOpen_;
    bool isMusicPlay_;

    SoundSystem();
    ~SoundSystem() { Close(); }

    void CreateShareBuffer_(typeSound type);
    void DeleteShareBuffer_(typeSound type);
    void AddShareBuffer_(typeSound type);
    OGuint GetShareBuffer_(typeSound type);

    // Translate type of share buffer in file name
    std::string SoundTypeToName_(typeSound type);
};

#endif // SOUNDSYSTEM_H
