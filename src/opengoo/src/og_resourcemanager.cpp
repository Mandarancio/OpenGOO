#include <fstream>

#include <QFile>
#include <QImage>
#include <QString>

#include "GameConfiguration/binltlfile.h"
#include "GameConfiguration/wog_text.h"
#include "GameConfiguration/wog_resources.h"
#include "GameConfiguration/animationdata.h"
#include "GameConfiguration/binanimparser.h"

#include <logger.h>

#include "og_resourcemanager.h"
#include "og_resourceconfig.h"
#include "og_data.h"

OGResourceManager::OGResourceManager()
{
    m_Music.second = og::audio::Music::Create();
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

bool OGResourceManager::ParseTextFile(const QString& a_filename, const QString& a_language)
{
    m_text.reset(OGData::GetText(a_filename, a_language));
    return m_text.get();
}

bool OGResourceManager::ParseFxFile(const QString& a_filename)
{
    m_effects.reset(OGData::GetEffects(a_filename));
    return m_effects.get();
}

og::ImageSourceSPtr OGResourceManager::CreateImageSource(const QString& a_filename)
{
#ifdef Q_OS_MAC
    auto fn = a_filename + ".png.binltl";
    if (!QFile::exists(fn))
    {
        return std::make_shared<og::ImageSource>();
    }

    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly))
    {
        return std::make_shared<og::ImageSource>();
    }

    auto data = BinLtlFile::Decompress(file);
    return std::make_shared<og::ImageSource>(data);
#else
    return std::make_shared<og::ImageSource>(a_filename + ".png");
#endif
}

og::ImageSourceSPtr OGResourceManager::GetImageSourceById(const QString& a_id)
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

    auto is = CreateImageSource(filename);
    m_imageSources[a_id] = is;
    return is;
}

template<typename T>
bool OGResourceManager::Load(T& a_data, const QString& a_filename)
{
    typename T::element_type::Conf conf;
    if (!conf.Open(a_filename))
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

const WOGBall* OGResourceManager::GetBallByType(const QString& a_type)
{
    if (a_type.isEmpty())
    {
        return nullptr;
    }

    auto it = m_balls.find(a_type);
    if (it != m_balls.end())
    {
        return it.value().get();
    }
    
    WOGBallPtr ball;
    QString path = "res/balls/" + a_type + "/balls.xml";
    Load(ball, path);
    if (!ball)
    {
        return nullptr;
    }

    ParseResourceFile("res/balls/" + a_type + "/resources.xml");

    auto sp = WOGBallSPtr(ball.release());
    m_balls[a_type] = sp;

    return sp.get();
}

og::audio::SoundSource* OGResourceManager::AddSoundSource(const QString a_id)
{
    foreach (auto& res, m_resources)
    {
        auto filename = res->GetSound(a_id);
        if (!filename.isEmpty())
        {
            filename.append(".ogg");
            auto it = m_soundSources.insert(a_id, og::audio::SoundSource(filename.toStdString()));;
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
        return std::make_shared<og::audio::Sound>(it.value());
    }

    if (auto src = AddSoundSource(a_id))
    {
        return std::make_shared<og::audio::Sound>(*src);
    }

    return nullptr;
}

og::audio::Music* OGResourceManager::GetMusic(const QString& a_id)
{
    if (m_Music.first == a_id)
    {
        return m_Music.second.get();
    }

    foreach (auto& res, m_resources)
    {
        auto filename = res->GetSound(a_id);
        if (!filename.isEmpty())
        {
            m_Music.first = filename + QLatin1String(".ogg");
            m_Music.second->OpenFile(m_Music.first.toStdString());
            return m_Music.second.get();
        }
    }

    return nullptr;
}

QString OGResourceManager::GetText(const QString& aId)
{
    return m_text->string.value(aId);
}

WOGEffect* OGResourceManager::GetEffect(const QString& aId)
{
    auto it = m_effects->find(aId);
    return (it != m_effects->end() ? it->get() : nullptr);
}

AnimationData* OGResourceManager::GetAnimation(const QString& aId)
{
    auto it = mAnimations.find(aId);
    if (it != mAnimations.end())
    {
        return (*it).get();
    }

    auto path = QString("res/anim/%1.anim.binltl").arg(aId);
    auto anim = BinAnimParser::Parse(path);
    it = mAnimations.insert(aId, std::move(anim));
    return it->get();
}

const og::IFont* OGResourceManager::GetFont(const QString& aId)
{
    auto it = mFonts.find(aId);
    if (it != mFonts.end())
    {
        return it.value().get();
    }

    it = mFonts.insert(aId, nullptr);
    return it.value().get();
}
