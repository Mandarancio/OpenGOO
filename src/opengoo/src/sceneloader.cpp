#include <QHash>

#include "OGLib/util.h"

#include "sceneloader.h"

#include "GameEngine/og_gameengine.h"
#include "GameEngine/scene.h"
#include "GameEngine/Particles/particleemiterfactory.h"
#include "GameEngine/Particles/particlesystem.h"
#include "GameEngine/Particles/particle.h"
#include "GameEngine/Particles/pointparticleemiter.h"
#include "GameEngine/Particles/ambientparticleemiter.h"

#include "GameConfiguration/animationdata.h"

#include "PhysicsEngine/og_physicsengine.h"

#include "og_utils.h"
#include "og_sprite.h"

#include "entities/og_ball.h"
#include "entities/scenelayer.h"
#include "entities/gamecontroller.h"
#include "entities/exit.h"
#include "entities/pipe.h"
#include "entities/strand.h"
#include "entities/compositegeom.h"

#include "entityfactory.h"

#include "animator.h"
#include "scaleanimation.h"
#include "translateanimation.h"
#include "rotationanimation.h"
#include "camzoomanimation.h"
#include "sequentialanimationgroup.h"

#include "opengoo.h"
#include "og_windowcamera.h"
#include "og_resourcemanager.h"

#include "configloader.h"

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
        , mFactory(CreateFactory())
    {
        mEntityFactory.SetPhysicsEngine(aScene.GetPhysicsEngine());
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

    void Process(const WOGCircle&);

    void Process(const WOGRectangle&);

    void Process(const WOGLine&);

    void Process(const WOGLinearForceField&);

    void Process(const WOGRadialForceField&);

    void Process(const WOGBallInstance&);

    void Process(const WOGStrand&);

    void Process(const WOGCompositeGeom&);

    template<typename T>
    void Process(const T& aData)
    {
        foreach(const auto& d, aData)
        {
            Process(d);
        }
    }

    static void SetupCamera(const WOGPoi& aPoi)
    {
        auto startX = aPoi.position.x();
        auto startY = aPoi.position.y();
        auto zoom = 1.0f / aPoi.zoom;
        GE->GetCamera()->SetPosition(startX, startY);
        GE->GetCamera()->SetZoom(zoom);
        GE->GetCamera()->SetAnimator(nullptr);
    }

    static std::shared_ptr<TranslateAnimation> CreateTranslateAnimation(float aDuration, const QPointF& aStart, const QPointF& aEnd)
    {
        int duration = aDuration * 1000;
        return std::make_shared<TranslateAnimation>(GE->GetCamera(), duration, aStart, aEnd);
    }

    static std::shared_ptr<CamZoomAnimation> CreateCamZoomAnimation(float aDuration, float aStart, float aEnd)
    {
        int duration = aDuration * 1000;
        return std::make_shared<CamZoomAnimation>(duration, 1.0f / aStart, 1.0f / aEnd);
    }

    void InitGlobalGravity(const QPointF& aGravity)
    {
        mScene.GetPhysicsEngine()->SetGravity(aGravity.x(), aGravity.y());
    }

    std::shared_ptr<Ball> AddBall(const QString& aId, std::shared_ptr<Ball> aBall)
    {
        assert(mBalls.count(aId) == 0);
        mBalls[aId] = aBall;
        return aBall;
    }

    std::shared_ptr<Ball> GetBallById(const QString& aId)
    {
        assert(mBalls.count(aId) == 1);
        return mBalls.value(aId);
    }

    static WOGBallStrand* GetStrandDefByBallType(const QString& aId)
    {
        if (auto ballDef = GE->GetResourceManager()->GetBallByType(aId))
        {
            return ballDef->strand.get();
        }

        return nullptr;
    }

    std::shared_ptr<og::IParticleEmiterFactory> CreateFactory()
    {
        return std::make_shared<og::ParticleEmiterFactory>();
    }

    std::shared_ptr<og::ParticleSystem> CreateParticleSystem(const QVector2D& aPosition, float aDepth)
    {
        return std::make_shared<og::ParticleSystem>(aPosition, aDepth, mFactory);
    }

private:
    og::Scene& mScene;
    EntityFactory mEntityFactory;
    QHash<QString, std::shared_ptr<Ball>> mBalls;
    std::shared_ptr<og::IParticleEmiterFactory> mFactory;
};

void SceneLoaderHelper::Process(const WOGCamera& aCamera)
{
    const auto& poi = aCamera.poi;
    if (poi.size() == 0)
    {
        return;
    }
    if (poi.size() == 1)
    {
        SetupCamera(poi.back());
    }
    else
    {
        SetupCamera(poi.front());

        auto animator = std::make_shared<Animator>();
        auto group = std::make_shared<SequentialAnimationGroup>();

        for (int i = 1; i < poi.size(); ++i)
        {
            group->AddAnimation(CreateTranslateAnimation(poi[i].traveltime, poi[i - 1].position, poi[i].position));
            group->AddAnimation(CreateCamZoomAnimation(poi[i].traveltime, poi[i - 1].zoom, poi[i].zoom));
        }

        animator->AddAnimationGroup(group)->Start(false);
        GE->GetCamera()->SetAnimator(animator);
    }
}

void SceneLoaderHelper::Process(const WOGBallInstance& aBallInstance)
{
    if (auto e = GetEntityFactory().CreateBall(aBallInstance))
    {
        auto b = AddBall(aBallInstance.id, e);
        mScene.AddEntity(b);
    }
}

void SceneLoaderHelper::Process(const WOGStrand& aStrand)
{
    auto b1 = GetBallById(aStrand.gb1);
    auto b2 = GetBallById(aStrand.gb2);
    b1->SetAttached(true);
    b2->SetAttached(true);

    auto strandDef = GetStrandDefByBallType(b1->GetName());
    if (!strandDef)
    {
        strandDef = GetStrandDefByBallType(b2->GetName());
    }

    if (!strandDef)
    {
        return;
    }

    if (auto entity = GetEntityFactory().CreateStrand(b1, b2, *strandDef))
    {
        b1->AddStrand(entity.get());
        b2->AddStrand(entity.get());
        mScene.AddEntity(entity);
    }
}

void SceneLoaderHelper::Process(const WOGLevel& aLevel)
{
    auto gc = GetEntityFactory().CreateGameController();
    if (auto music = GE->GetResourceManager()->GetMusic(aLevel.music))
    {
        gc->SetMusic(music);
    }

    foreach (const auto& id, aLevel.loopsound)
    {
        if (auto snd = GE->GetResourceManager()->GetSound(id))
        {
            snd->SetLoop(true);
            gc->AddLoopSound(snd);
        }
    }

    if (!aLevel.camera.empty())
    {
        auto aspect = (!(GE->getWidth() % 4) && !(GE->getHeight() % 3)) ? WOGCamera::Normal : WOGCamera::WideScreen;

        auto it = std::find_if(aLevel.camera.begin(), aLevel.camera.end(),
                     [aspect](const WOGCamera& aCamera) { return aCamera.aspect == aspect; });

        if (it != aLevel.camera.end())
        {
            Process(*it);
        }
    }

    {
        std::shared_ptr<Pipe> pipe;
        if (aLevel.HasPipe())
        {
            pipe = GetEntityFactory().CreatePipe(aLevel.GetPipe());
            mScene.AddEntity(pipe);
        }

        if (aLevel.HasLevelExit())
        {
            auto exit = GetEntityFactory().CreateExit(aLevel.GetLevelExit());
            exit->AddListener(pipe.get());
            exit->AddListener(gc.get());
            gc->SetExitPosition(aLevel.GetLevelExit().pos);
            gc->InitExit(aLevel.GetLevelExit().pos, aLevel.GetLevelExit().radius, mScene.GetPhysicsEngine()->GetRatio());
            gc->SetBallsRequired(aLevel.ballsrequired);
            mScene.AddEntity(exit);
        }
    }

    Process(aLevel.ball);
    Process(aLevel.strand);

    foreach (auto& b, mBalls)
    {
        gc->AddBall(b);
    }

    mScene.AddEntity(gc);
}

void SceneLoaderHelper::Process(const WOGSceneLayer& aSceneLayer)
{
    auto src = GE->GetResourceManager()->GetImageSourceById(aSceneLayer.image);
    auto spr = std::make_shared<OGSprite>(src);
    spr->CenterOrigin();
    spr->SetScaleX(aSceneLayer.scalex);
    spr->SetScaleY(aSceneLayer.scaley);
    spr->SetAngle(-aSceneLayer.rotation);
    spr->SetColorize(aSceneLayer.colorize);
    spr->SetAlpha(aSceneLayer.alpha);

    auto e = std::make_shared<SceneLayer>(aSceneLayer.x, -aSceneLayer.y, spr);
    e->SetDepth(aSceneLayer.depth);

    if (!aSceneLayer.anim.isEmpty())
    {
        AnimationData* ad = GE->GetResourceManager()->GetAnimation(aSceneLayer.anim);
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
                                                                  QPointF(frame[i].x, frame[i].y),
                                                                  QPointF(frame[next].x, frame[next].y));
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

            anim->AddAnimationGroup(ag)->Start(true);
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
        if (auto e = GetEntityFactory().CreateLabel(aLabel))
        {
            mScene.AddEntity(e);
        }
    }
}

void SceneLoaderHelper::Process(const WOGScene::WOGParticle& aParticle)
{
    if (auto effect = GE->GetResourceManager()->GetEffect(aParticle.effect))
    {
        const QVector2D pos(aParticle.position.x(), -aParticle.position.y());
        auto ps = CreateParticleSystem(pos, aParticle.depth);

        og::ParticleEmiterDefination ped;
        ped.type = effect->GetType();
        ped.maxparticles = effect->maxparticles;
        ped.margin = effect->margin;
        ped.rate = effect->rate;
        ped.timeoutInterval = 0.5f * GE->getFrameRate();
        if (auto emiter = ps->CreateEmiter(ped))
        {
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
                    auto src = GE->GetResourceManager()->GetImageSourceById(img);
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

void SceneLoaderHelper::Process(const WOGCircle& aCircle)
{
    if (auto e = GetEntityFactory().CreateCircle(aCircle))
    {
        mScene.AddEntity(e);
    }
}

void SceneLoaderHelper::Process(const WOGRectangle& aRect)
{
    if (auto e = GetEntityFactory().CreateRect(aRect))
    {
        mScene.AddEntity(e);
    }
}

void SceneLoaderHelper::Process(const WOGLine& aLine)
{
    if (auto e = GetEntityFactory().CreateLine(aLine))
    {
        mScene.AddEntity(e);
    }
}

void SceneLoaderHelper::Process(const WOGCompositeGeom& aDef)
{
    if (auto e = GetEntityFactory().CreateCompositeGeom(aDef))
    {
        mScene.AddEntity(e);
    }
}

void SceneLoaderHelper::Process(const WOGLinearForceField& aForceField)
{
    if (aForceField.width == 0)
    {
        InitGlobalGravity(aForceField.force);
    }
}

void SceneLoaderHelper::Process(const WOGRadialForceField& /*aForceField*/)
{
}

void SceneLoaderHelper::Process(const WOGScene& aScene)
{
    auto w = std::abs(aScene.maxx) + std::abs(aScene.minx);
    auto h = std::abs(aScene.maxy) + std::abs(aScene.miny);

    mScene.SetSize(w, h);
    mScene.SetPosition(aScene.minx, aScene.miny);

    GE->setBackgroundColor(aScene.backgroundcolor);

    Process(aScene.sceneLayer);
    Process(aScene.buttongroup);
    Process(aScene.label);
    Process(aScene.particle);

    Process(aScene.linearforcefield);
    Process(aScene.radialforcefield);
    Process(aScene.circle);
    Process(aScene.line);
    Process(aScene.rectangle);
    Process(aScene.compositegeom);
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

    LoadConfig("resrc", [](const QString& aPath){ return GE->GetResourceManager()->ParseResourceFile(aPath); });

    std::unique_ptr<WOGScene> scene;
    LoadConfig("scene", [&scene](const QString& aPath)
    {
        ConfigLoader::Load(scene, aPath);
        return scene.get();
    });

    SceneLoaderHelper helper(aScene);

    helper.Process(*scene);

    std::unique_ptr<WOGLevel> level;
    LoadConfig("level", [&level](const QString& aPath)
    {
        ConfigLoader::Load(level, aPath);
        return level.get();
    });

    helper.Process(*level);
}
