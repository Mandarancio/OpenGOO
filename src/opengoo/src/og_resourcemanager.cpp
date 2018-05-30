#include <QFile>
#include <QImage>
#include <QString>

#include "GameConfiguration/binltlfile.h"
#include "GameConfiguration/wog_resources.h"
#include "GameConfiguration/animationdata.h"
#include "GameConfiguration/binanimparser.h"

#include <logger.h>

#include "og_resourcemanager.h"

#include "configloader.h"

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
    ConfigLoader::Load(res, filename);
    if (!res)
    {
        logWarn("Could not load file:" + filename);
        return false;
    }

    m_resources[filename] = res;

    return true;
}

bool OGResourceManager::ParseTextFile(const QString& a_filename, const QString& a_language)
{
    m_text.clear();
    std::unique_ptr<WOGText> text;
    ConfigLoader::Load(text, a_filename);
    if (!text)
    {
        return false;
    }

    if (text->HasLanguage(a_language))
    {
        m_text = text->GetTextMap(a_language);
    }
    else if (text->HasLanguage("text"))
    {
        m_text = text->GetTextMap("text");
    }

    return !m_text.empty();
}

bool OGResourceManager::ParseFxFile(const QString& a_filename)
{
    m_effects.reset();
    ConfigLoader::Load(m_effects, a_filename);
    return m_effects.get();
}

bool OGResourceManager::ParseMaterialsFile(const QString& a_filename)
{
    mMaterials.clear();
    std::unique_ptr<WOGMaterialList> materials;
    ConfigLoader::Load(materials, a_filename);
    if (!materials)
    {
        return false;
    }

    foreach (const auto& m, *materials)
    {
        mMaterials[m.id] = m;
    }

    return true;
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
    ConfigLoader::Load(ball, path);
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
    return m_text.value(aId);
}

const WOGEffect* OGResourceManager::GetEffect(const QString& aId)
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

const WOGMaterial* OGResourceManager::GetMaterial(const QString& aId)
{
    auto it = mMaterials.find(aId);
    if (it == mMaterials.end())
    {
        return nullptr;
    }

    return &(*it);
}
