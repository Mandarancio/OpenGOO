#include "soundsystem.h"

SoundSystem* SoundSystem::m_instance_ = 0;

SoundSystem::SoundSystem()
{
    isOpen_ = false;
    player_ = 0;
    isMusicPlay_ = false;
}

SoundSystem* SoundSystem::GetInstance()
{
    if (!m_instance_) { m_instance_ = new SoundSystem; }

    return m_instance_;
}

void SoundSystem::Open()
{
    if (isOpen_) { return; }

    if (OGAlut::Init())
    {
        isOpen_ = true;
        isFailbit_ = false;
        soundSourceID_ = 1;
        player_ = 0;
    }
    else
    {
        isOpen_ = false;
        isFailbit_ = true;
    }
}

void SoundSystem::Close()
{
    if (isOpen_)
    {
        isOpen_ = false;
        CloseBGMusic();
        if(!soundObjects_.empty())
        {
            soundObjects_.clear();
            shareBuffers_.clear();
        }

        OGAlut::Exit();
    }
}

int SoundSystem::Create(typeSound type)
{
    if (!isOpen_) { return NONETYPE; }

    CreateShareBuffer_(type);
    OGuint buffer = GetShareBuffer_(type);

    if (buffer == -1) { return NONETYPE; }

    OGuint source = OGAlut::CreateSource();
    OGAlut::AttachBuffer(source, buffer);

    if (!soundObjects_.empty()) { soundSourceID_++; }

    soundObjects_.push_back(OGSoundObject(soundSourceID_, source, type));

    return soundSourceID_;
}

void SoundSystem::Delete(int id)
{
    if (!isOpen_ || id == NONETYPE) { return; }

    for (std::list<OGSoundObject>::iterator it=soundObjects_.begin(); it !=  soundObjects_.end(); it++)
    {
        if (it->id == id)
        {
            OGAlut::DeleteSource(it->source);
            DeleteShareBuffer_((typeSound)it->type);
            soundObjects_.erase(it);

            return;
        }
    }
}

void SoundSystem::CreateShareBuffer_(typeSound type)
{
    std::string path("resources/sounds/");
    std::string filename(SoundTypeToName_(type));
    std::string ext(".wav");
    std::string fullname(path + filename + ext);
    OGuint buffer;

    if (filename.compare("NONETYPE") == 0) { return; }

    if (shareBuffers_.empty())
    {
        buffer = OGAlut::CreateBufferFromFile((OGpchar)fullname.c_str());
        shareBuffers_.push_back(OGShareBuffer(type, buffer));

        return;
    }

    for (std::list<OGShareBuffer>::iterator it=shareBuffers_.begin(); it !=  shareBuffers_.end(); it++)
    {
        if (it->type == type)
        {
            it->counter = it->counter + 1;

            return;
        }
    }

    buffer = OGAlut::CreateBufferFromFile((OGpchar)fullname.c_str());
    shareBuffers_.push_back(OGShareBuffer(type, buffer));
}

void SoundSystem::DeleteShareBuffer_(typeSound type)
{  
    for (std::list<OGShareBuffer>::iterator it=shareBuffers_.begin(); it !=  shareBuffers_.end(); it++)
    {
        if (it->type == type)
        {
            it->counter = it->counter - 1;
            if (it->counter == 0)
            {
                OGAlut::DeleteBuffer(it->buffer);
                shareBuffers_.erase(it);

            }

            return;
        }
    }
}

OGuint SoundSystem::GetShareBuffer_(typeSound type)
{
    if (shareBuffers_.empty()) { return -1; }

    for (std::list<OGShareBuffer>::iterator it=shareBuffers_.begin(); it !=  shareBuffers_.end(); it++)
    {
        if (it->type == type)
        {
            return it->buffer;
        }
    }

    return -1;
}

std::string SoundSystem::SoundTypeToName_(typeSound type)
{   
    std::string name;

    switch(type)
    {
    case SCREAM:
        name = "scream";
        break;
    case BOING:
        name = "boing";
        break;
    case POP:
        name = "pop";
        break;
    case CAPTURED:
        name = "captured";
        break;
    case DRAG:
        name = "drag";
        break;
    default:
        name = "NONETYPE";
        break;
    }

    return name;
}

void SoundSystem::SetVolume(int id, float volume)
{

    if (!isOpen_ || id == NONETYPE)

    for (std::list<OGSoundObject>::iterator it=soundObjects_.begin(); it !=  soundObjects_.end(); it++)
    {
        if (it->id == id)
        {
            OGAlut::SetVolume(it->source, volume);

            return;
        }
    }
}

void SoundSystem::SetPitch(int id, float pitch)
{
    if (!isOpen_ || id == NONETYPE) { return; }

    for (std::list<OGSoundObject>::iterator it =  soundObjects_.begin(); it !=  soundObjects_.end(); it++)
    {
        if (it->id == id)
        {
            OGAlut::SetPitch(it->source, pitch);

            return;
        }
    }
}

void SoundSystem::SetCenter(QPoint p)
{
    center_ = p;
}

void SoundSystem::Play(int id)
{
    if (!isOpen_ || id == NONETYPE) { return; }

    for (std::list<OGSoundObject>::iterator it=soundObjects_.begin(); it !=  soundObjects_.end(); it++)
    {
        if (it->id == id)
        {
            OGAlut::Play(it->source);

            return;
        }
    }
}

void SoundSystem::Stop(int id)
{
    if (!isOpen_ || id == NONETYPE) { return; }

    for (std::list<OGSoundObject>::iterator it=soundObjects_.begin(); it !=  soundObjects_.end(); it++)
    {
        if (it->id == id)
        {
            OGAlut::Stop(it->source);

            return;
        }
    }
}

void SoundSystem::Pause(int id)
{
    if(!isOpen_ && id == NONETYPE) { return; }

    for (std::list<OGSoundObject>::iterator it=soundObjects_.begin(); it !=  soundObjects_.end(); it++)
    {
        if (it->id == id)
        {
            OGAlut::Pause(it->source);

            return;
        }
    }
}

int SoundSystem::Status(int id)
{
    if (!isOpen_ || id == NONETYPE) { return -1; }

    for (std::list<OGSoundObject>::iterator it =  soundObjects_.begin(); it !=  soundObjects_.end(); it++)
    {
        if (it->id == id) { return OGAlut::GetState(it->source); }
    }

    return -1;
}

void SoundSystem::SetPosition(int id, QPoint p)
{
    if (!isOpen_ || id == NONETYPE) { return; }

    float x = p.x()-center_.x();
    float y = p.y()-center_.y();

    for (std::list<OGSoundObject>::iterator it=soundObjects_.begin(); it !=  soundObjects_.end(); it++)
    {
        if (it->id == id)
        {
            OGAlut::SetPosition(it->source, x, y);
            return;
        }
    }
}

void SoundSystem::OpenBGMusic(const OGpchar filename, bool loopmode, bool streammode)
{
    if (!isOpen_)
    {
        player_ = 0;
        return;
    }

    if (player_ == 0)
    {
        player_ = new OGMPlayer;
        player_->Open(filename, loopmode, streammode);
    }
}

void SoundSystem::CloseBGMusic()
{
    delete player_;
    player_ = 0;
    isMusicPlay_ = false;
}

void SoundSystem::PlayBGMusic()
{
    if (player_)
    {
        player_->Play();
        isMusicPlay_ = true;
    }
}

void SoundSystem::StopBGMusic()
{
    if (player_)
    {
        player_->Stop();
        isMusicPlay_ = false;
    }
}
void SoundSystem::PauseBGMusic()
{
    if (player_)
    {
        player_->Pause();
        isMusicPlay_ = false;
    }
}
