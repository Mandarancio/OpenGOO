#include "sceneloader.h"
#include "GameEngine/og_gameengine.h"
#include "GameEngine/scene.h"
#include "og_utils.h"
#include "og_levelconfig.h"
#include "og_sceneconfig.h"
#include "wog_level.h"
#include "wog_scene.h"

#include "entities/og_ball.h"
#include "entityfactory.h"

#include "spritefactory.h"

struct SceneLoaderHelper
{
    SceneLoaderHelper(og::Scene& aScene)
        : mScene(aScene)
        , mEntityFactory(*GE->getResourceManager())
    {
    }

    void processLevel(const WOGLevel& aLevel);

    void processScene(const WOGScene& aScene);

    void processWOGSceneLayer(const WOGSceneLayer& aSceneLayer);

    og::Scene& mScene;
    EntityFactory mEntityFactory;
};

void SceneLoaderHelper::processLevel(const WOGLevel& aLevel)
{
}

void SceneLoaderHelper::processWOGSceneLayer(const WOGSceneLayer& aSceneLayer)
{
    auto src = SpriteFactory::CreateImageSource(aSceneLayer.image);
    auto spr = std::make_shared<OGSprite>(src);
    spr->CenterOrigin();
    spr->SetScale(aSceneLayer.scale);
    spr->SetAngle(-aSceneLayer.rotation);
    spr->SetColorize(aSceneLayer.colorize);
    spr->SetAlpha(aSceneLayer.alpha);

    auto e = std::make_shared<og::Entity>(aSceneLayer.position.x(), -aSceneLayer.position.y(), spr);
    e->SetDepth(aSceneLayer.depth);
    mScene.AddEntity(e);
}

void SceneLoaderHelper::processScene(const WOGScene& aScene)
{
    foreach (const auto sl, aScene.sceneLayer)
    {
        processWOGSceneLayer(*sl);
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

    fullPath = path + "level";
    OGLevelConfig lc(fullPath);
    if (!LoadConf(lc))
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

    SceneLoaderHelper helper(aScene);
    helper.processScene(*sc.Parser());

    return true;
}
