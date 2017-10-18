#include <QFile>
#include <QImage>
#include <QString>

#include <logger.h>

#include "og_resourcemanager.h"
#include "og_resourceconfig.h"
#include "wog_resources.h"
#include "og_data.h"

using namespace og;

OGResourceManager::OGResourceManager()
    : m_file("og-log.txt")
{
    m_previous = audio::SoundUtil::SetStreamBuffer(m_file.rdbuf());
}

OGResourceManager::~OGResourceManager()
{
    audio::SoundUtil::SetStreamBuffer(m_previous);
}

bool OGResourceManager::ParseResourceFile(const QString& a_filename)
{
    if (m_resources.contains(a_filename))
        return true;

    QString filename = a_filename;
    if (!QFile::exists(filename))
    {
        filename += ".bin";
        if (!QFile::exists(filename))
        {
            logWarn("File:" + a_filename + " not exists");
            return false;
        }
    }

    WOGResourcesSPtr res;
    if (!Load(res, filename))
    {
        logWarn("Could not load file:" + filename);
        return false;
    }

    m_resources[filename] = res;

    return true;
}

OGResourceManager::ImageSourceSPtr OGResourceManager::GetImageSourceById(const QString& a_id)
{
    if (auto is = m_imageSources.value(a_id))
        return is;

    QString filename;
    foreach (auto& res, m_resources)
    {
        filename = res->GetImage(a_id);
        if (!filename.isEmpty())
            break;
    }

    if (filename.isEmpty())
    {
        logWarn("Could not find id:" + a_id);
        return nullptr;
    }

    m_imageSources[a_id] = std::make_shared<ImageSource>(filename + ".png");

    return m_imageSources.value(a_id);
}

template<typename T>
bool OGResourceManager::Load(T& a_data, const QString& a_filename)
{
    typename T::element_type::Conf conf(a_filename);
    if (!conf.Open())
    {
        logWarn("Could not open file:" + a_filename);
        return false;
    }

    if (!conf.Read())
    {
        logWarn("Could not read file:" + a_filename);
        return false;
    }

    a_data.reset(conf.Parser());

    return true;
}

WOGBall* OGResourceManager::GetBallByType(const QString& a_type)
{
    if (auto ball = m_balls.value(a_type))
        return ball.get();
    
    WOGBallPtr ball;
    QString path = "res/balls/" + a_type + "/balls.xml";
    Load(ball, path);
    if (!ball)
        return nullptr;

    ParseResourceFile("res/balls/" + a_type + "/resources.xml");

    auto sp = WOGBallSPtr(ball.release());
    m_balls[a_type] = sp;

    return sp.get();
}

og::audio::SoundSource* OGResourceManager::AddSoundSource(const QString a_id)
{
    static int count = 0;

    foreach (auto& res, m_resources)
    {
        auto filename = res->GetSound(a_id);
        if (!filename.isEmpty())
        {
            filename.append(".ogg");
            auto it = m_soundSources.insert(a_id, og::audio::SoundSource(filename.toStdString()));
            ++count;
            qDebug() << count << " id:" << a_id;
            return &(it.value());
        }
    }

    return nullptr;
}

const og::audio::SoundSource* OGResourceManager::GetSoundSource(const QString& a_id)
{
    auto it = m_soundSources.find(a_id);
    if (it != m_soundSources.end())
    {
        return &it.value();
    }

    return AddSoundSource(a_id);
}

SoundSPtr OGResourceManager::GetSound(const QString& a_id)
{
    auto it = m_soundSources.find(a_id);
    if (it != m_soundSources.end())
    {
//        qDebug() << ++count;
        return std::make_shared<og::audio::Sound>(it.value());
    }

    if (auto src = AddSoundSource(a_id))
    {
//        qDebug() << ++count;
        return std::make_shared<og::audio::Sound>(*src);
    }

    return nullptr;
}

MusicSPtr OGResourceManager::GetMusic(const QString& a_id)
{
    if (m_Music.id == a_id)
    {
        return m_Music.audio;
    }

    foreach (auto& res, m_resources)
    {
        auto filename = res->GetSound(a_id);
        if (!filename.isEmpty())
        {
            m_Music.id = filename;
            filename.append(".ogg");
            m_Music.audio = std::make_shared<og::audio::Music>(filename.toStdString());
            return m_Music.audio;
        }
    }

    return nullptr;
}
