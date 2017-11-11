#include "sceneloader.h"
#include "GameEngine/og_gameengine.h"
#include "GameEngine/scene.h"
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
        , mResouceManger(GE->getResourceManager())
        , mEntityFactory(*mResouceManger)
    {
    }

    void processLevel(const WOGLevel&);

    void processScene(const WOGScene&);

    void processSceneLayer(const WOGSceneLayer&);

    void processButtonGroup(const WOGButtonGroup&);

    void processButton(const WOGButton&);

    void processCamera(const WOGCamera&);

    void processLabel(const WOGLabel&);

    void processParticle(const WOGScene::WOGParticle&);

    og::Scene& mScene;
    og::IResourceManager* mResouceManger;
    EntityFactory mEntityFactory;
};

void SceneLoaderHelper::processCamera(const WOGCamera& aCamera)
{
    const auto& poi = aCamera.poi.back();
    auto cam = GE->getCamera();
    cam->SetPosition(poi.position.x(), poi.position.y());
    cam->SetZoom(1 /poi.zoom);
}

void SceneLoaderHelper::processLevel(const WOGLevel& aLevel)
{
    if (!aLevel.camera.empty())
    {
        auto dr = std::div(GE->getWidth(), GE->getHeight());
        auto aspect = (dr.quot == 4 && dr.rem == 3) ? WOGCamera::Normal : WOGCamera::WideScreen;
        auto it = std::find_if(aLevel.camera.begin(), aLevel.camera.end(),
                     [aspect](const WOGCamera& aCamera) { return aCamera.aspect == aspect; });

        if (it != aLevel.camera.end())
        {
            processCamera(*it);
        }
    }
}

void SceneLoaderHelper::processSceneLayer(const WOGSceneLayer& aSceneLayer)
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

void SceneLoaderHelper::processButton(const WOGButton& aButton)
{
    mScene.AddEntity(mEntityFactory.CreateButton(aButton));
}

void SceneLoaderHelper::processButtonGroup(const WOGButtonGroup& aButtonGroup)
{
    foreach (const auto& btn, aButtonGroup.button)
    {
        processButton(btn);
    }
}

void SceneLoaderHelper::processLabel(const WOGLabel& aLabel)
{
    if (aLabel.id.isEmpty())
    {
        mScene.AddEntity(mEntityFactory.CreateLabel(aLabel));
    }
}

void SceneLoaderHelper::processParticle(const WOGScene::WOGParticle& aParticle)
{
    if (auto effect = ((OGResourceManager*)mResouceManger)->GetEffect(aParticle.effect))
    {
        qDebug() << aParticle.effect << effect->GetType();
    }
}

void SceneLoaderHelper::processScene(const WOGScene& aScene)
{
    GE->setBackgroundColor(aScene.backgroundcolor);

    foreach (const auto& sl, aScene.sceneLayer)
    {
        processSceneLayer(sl);
    }

    foreach (const auto& bg, aScene.buttongroup)
    {
        processButtonGroup(bg);
    }

    foreach (const auto& l, aScene.label)
    {
        processLabel(l);
    }

    foreach (const auto& p, aScene.particle)
    {
        processParticle(p);
    }
}

template<typename T> bool LoadConf(T& config)
{
    if (!config.Open())
    {
        return false;
    }

    if (!config.Read())
    {
        return false;
    }

    return true;
}

bool SceneLoader::load(og::Scene &aScene)
{
    auto rm = GE->getResourceManager();
    auto path = QString("./res/levels/%1/%1.").arg(aScene.GetName());
    auto fullPath = path + "resrc";
    if (!rm->ParseResourceFile(fullPath))
    {
        logError("Could not load " + fullPath);
        return false;
    }

    fullPath = path + "scene";
    OGSceneConfig sc(fullPath);
    if (!LoadConf(sc))
    {
        logError("Could not load " + fullPath);
        return false;
    }

    fullPath = path + "level";
    OGLevelConfig lc(fullPath);
    if (!LoadConf(lc))
    {
        logError("Could not load " + fullPath);
        return false;
    }

    SceneLoaderHelper helper(aScene);
    auto scene = sc.Parser();
    helper.processScene(*scene);

    auto level = lc.Parser();
    helper.processLevel(*level);

    return true;
}
