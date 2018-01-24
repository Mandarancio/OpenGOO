#include "sceneloader.h"
#include "GameEngine/og_gameengine.h"
#include "GameEngine/scene.h"
#include "GameEngine/Particles/particlesystem.h"
#include "GameEngine/Particles/particle.h"
#include "GameEngine/Particles/pointparticleemiter.h"
#include "GameEngine/Particles/ambientparticleemiter.h"
#include "GameConfiguration/animationdata.h"

#include "og_utils.h"
#include "wog_level.h"
#include "wog_scene.h"
#include "og_levelconfig.h"
#include "og_sceneconfig.h"

#include "entities/og_ball.h"
#include "entityfactory.h"

#include "spritefactory.h"
#include "animator.h"
#include "scaleanimation.h"
#include "translateanimation.h"
#include "rotationanimation.h"
#include "sequentialanimationgroup.h"
#include "entities/scenelayer.h"

#include "opengoo.h"
#include "og_windowcamera.h"
#include "og_resourcemanager.h"

template<>
inline void OptionalSetValue(std::pair<bool, og::ParticleDefination::AxialSinOffset>& aOptional,
                             const WOGEffect::WOGParticle::WOGAxialSinOffset& aValue)
{
    aOptional.first = true;
    auto& aso = aOptional.second;
    aso.SetAmplitude(aValue.amp);
    aso.SetFrequency(aValue.freq);
    aso.SetPhaseshift(aValue.phaseshift);
}

inline QString ToString(const QPointF& aPoint)
{
    return QString("(%1, %2)").arg(aPoint.x()).arg(aPoint.y());
}

inline QString ToString(bool aFlag)
{
    return aFlag ? "True" : "False";
}

inline QString ToString(const QStringList& aStrList)
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
        AnimationData* ad = GetResourceManager()->GetAnimation(aSceneLayer.anim);
        auto anim = std::make_shared<Animator>();
        foreach (const auto& entry, ad->transformFrame)
        {
            auto ag = std::make_shared<SequentialAnimationGroup>();
            const auto& frame = entry.frame;
            for (int i = 0; frame[i].nextFrameIndex != -1;)
            {
                auto next = frame[i].nextFrameIndex;
                int duration = ((ad->frameTime[next] - ad->frameTime[i]) * 1000) / std::fabs(aSceneLayer.animspeed);
                switch (entry.type)
                {
                case AnimationData::XFORM_SCALE:
                    {
                        auto a = std::make_shared<ScaleAnimation>(e.get(),
                                                                  duration,
                                                                  frame[i].x,
                                                                  frame[i].y,
                                                                  frame[next].x,
                                                                  frame[next].y);
                        ag->AddAnimation(a);
                    }
                    break;
                case AnimationData::XFORM_TRANSLATE:
                    {
                        auto a = std::make_shared<TranslateAnimation>(e.get(),
                                                                  duration,
                                                                  frame[i].x,
                                                                  frame[i].y,
                                                                  frame[next].x,
                                                                  frame[next].y);
                        ag->AddAnimation(a);
                    }
                    break;
                case AnimationData::XFORM_ROTATE:
                    {
                        auto a = std::make_shared<RotationAnimation>(e.get(),
                                                                  duration,
                                                                  frame[i].angle,
                                                                  frame[next].angle,
                                                                  IsNegative(aSceneLayer.animspeed));
                        ag->AddAnimation(a);
                    }
                    break;
                }

                i = next;
            }

            anim->AddAnimationGroup(ag)->Start();
        }

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
                em->SetMargin(effect->margin);
                em->SetTimeoutInterval(0.5f * GE->getFrameRate());
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

                foreach (const auto& axialsinoffset, particle.axialsinoffset)
                {
                    switch (axialsinoffset.axis)
                    {
                    case WOGEffect::WOGParticle::WOGAxialSinOffset::e_x:
                        OptionalSetValue(pd.xAxialSinOffset, axialsinoffset);
                        break;
                    case WOGEffect::WOGParticle::WOGAxialSinOffset::e_y:
                        OptionalSetValue(pd.yAxialSinOffset, axialsinoffset);
                        break;
                    }
                }

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
