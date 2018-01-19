#include "sceneloader.h"
#include "GameEngine/og_gameengine.h"
#include "GameEngine/scene.h"
#include "GameEngine/Particles/particlesystem.h"
#include "GameEngine/Particles/particle.h"
#include "GameEngine/Particles/pointparticleemiter.h"
#include "GameEngine/Particles/ambientparticleemiter.h"

#include "og_utils.h"
#include "wog_level.h"
#include "wog_scene.h"
#include "og_levelconfig.h"
#include "og_sceneconfig.h"

#include "entities/og_ball.h"
#include "entityfactory.h"

#include "spritefactory.h"
#include "animator.h"
#include "entities/scenelayer.h"

#include "opengoo.h"
#include "og_windowcamera.h"
#include "og_resourcemanager.h"

struct SceneLoaderHelper
{
    SceneLoaderHelper(og::Scene& aScene)
        : mScene(aScene)
        , mEntityFactory(*GetResourceManager())
    {
    }

    og::IResourceManager* GetResourceManager()
    {
        return GE->getResourceManager();
    }

    EntityFactory& GetEntityFactory()
    {
        return mEntityFactory;
    }

    void Process(const WOGLevel&);

    void Process(const WOGScene&);

    void Process(const WOGSceneLayer&);

    void Process(const WOGButtonGroup&);

    void Process(const WOGButton&);

    void Process(const WOGCamera&);

    void Process(const WOGLabel&);

    void Process(const WOGScene::WOGParticle&);

    template<typename T>
    void Process(const T& aData)
    {
        foreach(const auto& d, aData)
        {
            Process(d);
        }
    }

private:
    og::Scene& mScene;
    EntityFactory mEntityFactory;
};

void SceneLoaderHelper::Process(const WOGCamera& aCamera)
{
    const auto& poi = aCamera.poi.back();
    auto cam = GE->getCamera();
    cam->SetPosition(poi.position.x(), poi.position.y());
    cam->SetZoom(1 /poi.zoom);
}

void SceneLoaderHelper::Process(const WOGLevel& aLevel)
{
    if (!aLevel.camera.empty())
    {
        auto dr = std::div(GE->getWidth(), GE->getHeight());
        auto aspect = (dr.quot == 4 && dr.rem == 3) ? WOGCamera::Normal : WOGCamera::WideScreen;
        auto it = std::find_if(aLevel.camera.begin(), aLevel.camera.end(),
                     [aspect](const WOGCamera& aCamera) { return aCamera.aspect == aspect; });

        if (it != aLevel.camera.end())
        {
            Process(*it);
        }
    }
}

void SceneLoaderHelper::Process(const WOGSceneLayer& aSceneLayer)
{
    auto src = SpriteFactory::CreateImageSource(aSceneLayer.image);
    auto spr = std::make_shared<OGSprite>(src);
    spr->CenterOrigin();
    spr->SetScale(aSceneLayer.scale);
    spr->SetAngle(-aSceneLayer.rotation);
    spr->SetColorize(aSceneLayer.colorize);
    spr->SetAlpha(aSceneLayer.alpha);

    auto e = std::make_shared<SceneLayer>(aSceneLayer.position.x(), -aSceneLayer.position.y(), spr);
    e->SetDepth(aSceneLayer.depth);

    if (!aSceneLayer.anim.isEmpty())
    {
        // TODO Remove this test data
        auto anim = std::make_shared<Animator>(e.get(), aSceneLayer.animspeed);
        anim->AddTransformType(Animator::XFORM_ROTATE);
        anim->AddFrameTime(0);
        anim->AddFrameTime(0.5);
        anim->AddFrameTime(1);

        KeyFrame kf;
        kf.interpolation = KeyFrame::INTERPOLATION_LINEAR;

        kf.angle = 0;
        kf.nextFrameIndex = 1;
        anim->AddTransformFrame(kf);

        kf.angle = 180.0f;
        kf.nextFrameIndex = 2;
        anim->AddTransformFrame(kf);

        kf.angle = 360.0f;
        kf.nextFrameIndex = -1;
        anim->AddTransformFrame(kf);

        e->SetAnimator(anim);
    }

    if (!aSceneLayer.id.isEmpty())
    {
        e->SetVisible(false);
    }

    mScene.AddEntity(e);
}

void SceneLoaderHelper::Process(const WOGButton& aButton)
{
    mScene.AddEntity(GetEntityFactory().CreateButton(aButton));
}

void SceneLoaderHelper::Process(const WOGButtonGroup& aButtonGroup)
{
    foreach (const auto& btn, aButtonGroup.button)
    {
        Process(btn);
    }
}

void SceneLoaderHelper::Process(const WOGLabel& aLabel)
{
    if (aLabel.id.isEmpty())
    {
        mScene.AddEntity(GetEntityFactory().CreateLabel(aLabel));
    }
}

static QString ToString(const QPointF& aPoint)
{
    return QString("(%1, %2)").arg(aPoint.x()).arg(aPoint.y());
}

static QString ToString(bool aFlag)
{
    return aFlag ? "True" : "False";
}

static QString ToString(const QStringList& aStrList)
{
    QString str;
    QTextStream stream(&str);
    stream << "{";

    for (int i = 0; i < aStrList.size(); ++i)
    {
        stream << aStrList[i];
        if (i + 1 < aStrList.size())
        {
            stream << ", ";
        }
    }

    stream << "}";

    return str;
}

//static QString ToString(WOGEffect::WOGParticle::WOGAxialSinOffset::Axis axis)
//{
//    switch (axis)
//    {
//    case WOGEffect::WOGParticle::WOGAxialSinOffset::Axis::e_x:
//        return "X";
//    case WOGEffect::WOGParticle::WOGAxialSinOffset::Axis::e_y:
//        return "Y";
//    }

//    return "Unknown";
//}

//static QString ToString(const WOGEffect::WOGParticle::WOGAxialSinOffset& offset)
//{
//    QByteArray ba;
//    QDataStream stream(&ba, QIODevice::WriteOnly);
//    stream << "WOGEffect::WOGParticle::WOGAxialSinOffset("
//       << "axis:" << ToString(offset.axis)
//       << ", freq:" << offset.freq
//       << ", amp:" << offset.amp
//       << ", phaseshift:" << offset.phaseshift
//       << ")";
//    return ba;
//}

static QString ToString(const WOGEffect::WOGParticle& aParticle)
{
    QString str;
    QTextStream stream(&str);
    stream << "WOGParticle("
           << "scale=" << ToString(aParticle.scale)
           << ", finalscale[" << ToString(aParticle.finalscale.first) << "]=" << aParticle.finalscale.second
           << ", movedirvar=" << aParticle.movedirvar
           << ", image=" << ToString(aParticle.image)
           << ", directed=" << ToString(aParticle.directed)
           << ", movedir=" << aParticle.movedir
           << ", acceleration=" << ToString(aParticle.acceleration)
           << ", additive=" << ToString(aParticle.additive)
           << ", lifespan[" << ToString(aParticle.lifespan.first) << "]=" << ToString(aParticle.lifespan.second)
           << ", speed=" << ToString(aParticle.speed)
           << ", dampening[" << ToString(aParticle.dampening.first) << "]=" << aParticle.dampening.second
           << ", rotation=" << ToString(QPointF(aParticle.rotation))
           << ", rotspeed[" << ToString(aParticle.rotspeed.first) << "]="<< ToString(aParticle.rotspeed.second)
           << ", fade="<< ToString(aParticle.fade)
           << ", axialsinoffset[" << static_cast<int>(aParticle.axialsinoffset.size()) << "]"
           << ")";
    return str;
}

static QString ToString(og::ParticleEmiter::Type type)
{
    switch (type)
    {
    case og::ParticleEmiter::e_unknown:
        return"Unknown";
    case og::ParticleEmiter::e_point:
        return "Point";
    case og::ParticleEmiter::e_ambient:
        return "Ambient";
    case og::ParticleEmiter::e_user:
        return "User";
    }

    return "Unknown";
}

inline QDebug operator<< (QDebug dbg, const WOGEffect& effect)
{
    dbg.nospace() << "WOGEffect("
                  << "type=" << ToString(effect.GetType())
                  << ", margin=" << effect.margin
                  << ", maxparticles=" << effect.maxparticles
                  << ", rate=" << effect.rate
                  << ", particle[" << effect.particle.size() << "]";
    if (effect.particle.size() > 0)
    {
    dbg.noquote() << "={";
    foreach (auto& particle, effect.particle)
    {
        dbg << ToString(particle);
    }

    dbg << "}";
    }

    dbg << ")";

    return dbg.space();
}

void SceneLoaderHelper::Process(const WOGScene::WOGParticle& aParticle)
{
    assert(dynamic_cast<OGResourceManager*>(GetResourceManager()));

    if (auto effect = static_cast<OGResourceManager*>(GetResourceManager())->GetEffect(aParticle.effect))
    {
        const auto pos = QVector2D(aParticle.position.x(), -aParticle.position.y());
        auto ps = og::ParticleSystem::Create(pos, aParticle.depth);
        if (auto emiter = ps->CreateEmiter(effect->GetType(), effect->maxparticles))
        {
            if (auto em = dynamic_cast<og::PointParticleEmiter*>(emiter))
            {
                em->SetRate(effect->rate);
            }

            if (auto em = dynamic_cast<og::AmbientParticleEmiter*>(emiter))
            {
                qDebug() << aParticle.effect << *effect;
                em->SetMargin(effect->margin);
                em->SetTimeoutInterval(0.5 * GE->getFrameRate());
            }

            og::ParticleDefination pd;

            foreach (const auto& particle, effect->particle)
            {
                pd.finalScale = particle.finalscale;
                pd.lifespan = particle.lifespan;

                pd.direction = (particle.movedir < 0 ? 360 + particle.movedir : particle.movedir);
                pd.dirvar = particle.movedirvar;
                pd.SetSpeed(particle.speed);
                pd.acceleration = particle.acceleration;
                pd.rotation = particle.rotation;
                pd.rotationSpeed = particle.rotspeed;
                pd.isDirected = particle.directed;
                pd.dampening = particle.dampening;

                foreach (const auto& img, particle.image)
                {
                    auto src = SpriteFactory::CreateImageSource(img);
                    pd.AddImageSource(src);
                }

                pd.isAdditive = particle.additive;
                pd.SetScale(particle.scale);
                pd.shouldFade = particle.fade;

                emiter->AddParticleDefination(pd);
            }
        }

        mScene.AddEntity(ps);
    }
}

void SceneLoaderHelper::Process(const WOGScene& aScene)
{
    GE->setBackgroundColor(aScene.backgroundcolor);

    Process(aScene.sceneLayer);
    Process(aScene.buttongroup);
    Process(aScene.label);
    Process(aScene.particle);
}

void SceneLoader::Load(og::Scene& aScene)
{
    auto LoadConfig = [&aScene](const QString& aType, std::function<bool(const QString&)> aLoader)
    {
        const auto path = QString("./res/levels/%1/%1.%2").arg(aScene.GetName()).arg(aType);
        if (!aLoader(path))
        {
            throw std::runtime_error(qPrintable("Could not load file " + path));
        }
    };

    LoadConfig("resrc", [](const QString& aPath){ return GE->getResourceManager()->ParseResourceFile(aPath); });

    OGSceneConfig sc;
    LoadConfig("scene", [&sc](const QString& aPath){ return sc.Load(aPath); });

    OGLevelConfig lc;
    LoadConfig("level", [&lc](const QString& aPath){ return lc.Load(aPath); });

    SceneLoaderHelper helper(aScene);

    auto scene = sc.Parser();
    helper.Process(*scene);

    auto level = lc.Parser();
    helper.Process(*level);
}
