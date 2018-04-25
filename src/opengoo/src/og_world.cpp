#include <memory>

#include <QPainter>
#include <QFile>
#include <QtMath>

#include "og_world.h"
#include "logger.h"
#include "og_circle.h"
#include "og_rectangle.h"
#include "og_line.h"
#include "entities/og_ball.h"
#include "og_button.h"
#include "GameEngine/og_gameengine.h"
#include "og_windowcamera.h"
#include "og_strand.h"
#include "opengoo.h"
#include "OGLib/rectf.h"

#include <OGPhysicsEngine>
#include <OGContactListener>
#include "physicsenginefactory.h"

#include "gamedata.h"
#include "entityfactory.h"

using namespace og;

OGWorld::OGWorld(PhysicsEngineFactory& a_physicsEngineFactory,
                 EntityFactory& a_factory,
                 const QString& a_levelname,
                 const QString& a_language)
    : Scene(a_levelname, a_physicsEngineFactory.Create())
    ,  m_entityFactory(a_factory)
    , m_physicsEngine(a_physicsEngineFactory.Create())
    , m_language(a_language)
{

    m_entityFactory.SetPhysicsEngine(m_physicsEngine.get());

//    pLevelData_ = nullptr;
//    pSceneData_ = nullptr;
//    pResourcesData_[0] = nullptr;  // global resources
//    pResourcesData_[1] = nullptr;  // local resources
//    pTextData_[0] = nullptr;       // global text
//    pTextData_[1] = nullptr;       // local text
//    pMaterialData_ = nullptr;
//    pEffectsData_ = nullptr;
//    pCamera_ = nullptr;

//    pNearestBall_ = 0;
//    strandId_ = 0;
//    ballId_ = 0;

//    isPhysicsEngine_ = false;
//    isLevelLoaded_ = false;

    m_num_added = 0;
}

OGWorld::~OGWorld()
{
////    _ClearLocalData(); // clear local data

////    logInfo("Clear share data");

////    if (pResourcesData_[0])
////        delete pResourcesData_[0];

////    if (pTextData_[0])
////        delete pTextData_[0];

////    if (pMaterialData_)
////        delete pMaterialData_;

////    if (pEffectsData_)
////        delete pEffectsData_;

//    _ClearScene();
}

const QColor& OGWorld::GetBackgroundColor() const
{
    return m_backgroundColor;
}

float OGWorld::GetWidth() const
{
    return 0;
}

float OGWorld::GetHeight() const
{
    return 0;
}

//bool OGWorld::Initialize()
//{
//    logInfo("Loading share data...");

//    QString path = "./properties/materials.xml";

//    _LoadMaterials(path);

//    path = "./properties/resources.xml";

//    if (!_LoadResources(path, true))
//        return false;

//    path = "./properties/fx.xml";

//    _LoadFX(path);

//    path = "./properties/text.xml";

//    if (!_LoadText(path, true))
//        return false;

//    return true;
//}

bool OGWorld::Load()
{
//    logInfo("Loading ...");

//    if (isExist(levelName_))
//    {
//        QString path = "./res/levels/" + levelName_ + "/" + levelName_ + ".level";

//        if (!_LoadLevel(path))
//            return false;

//        path = "./res/levels/" + levelName_ + "/" + levelName_  + ".scene";

//        if (!_LoadScene(path))
//            return false;

//        path = "./res/levels/" + levelName_ + "/" + levelName_  + ".resrc";

//        if (!_LoadResources(path, false))
//            return false;
//    }

//    else
//    {
//        logWarn(QString("Wrong level name: \"%1\"").arg(levelName_));

//        return false;
//    }

    return true;
}

//void OGWorld::Reload()
//{
//    _ClearPhysics();
//    CreatePhysicsScene();
//}

//template<class Target, class Config>
//Target OGWorld::LoadConf(const QString &path)
//{
//    Config config(path);

//    if (config.Open())
//    {
//        if (config.Read())
//            return config.Parser();
//        else
//            logWarn("File " + path + " is corrupted");
//    }
//    else
//        logWarn("File " + path + " not found");

//    return 0;
//}

//bool OGWorld::_LoadFX(const QString &path)
//{
//    logInfo("effects file");

//    if (pEffectsData_)
//        return false;

//    pEffectsData_ = LoadConf<WOGEffects*, OGEffectConfig> (path);

//    if (pEffectsData_)
//        return true;
//    else
//        return false;
//}

//bool OGWorld::_LoadLevel(const QString path)
//{
//    logInfo("level file");

//    if (pLevelData_)
//        return false;

//    pLevelData_ = LoadConf<WOGLevel*, OGLevelConfig> (path);

//    return pLevelData_;
//}

//bool OGWorld::_LoadMaterials(const QString &path)
//{
//    logInfo("material file");

//    if (pMaterialData_)
//        return false;

//    pMaterialData_ = LoadConf<WOGMaterialList*, OGMaterialConfig> (path);

//    if (pMaterialData_)
//        return true;
//    else
//        return false;
//}

//bool OGWorld::_LoadResources(const QString &path, bool share)
//{
//    logInfo("resources file");

//    GE->getResourceManager()->ClearAllSoundSources();
//    GE->getResourceManager()->ParseResourceFile(path);

//    WOGResources* data;

//    if (share)
//        data = pResourcesData_[0];
//    else
//        data = pResourcesData_[1];

//    if (data)
//        return false;

//    if (share)
//        data = OGData::GetResources(path);
//    else
//        data = OGData::GetResources(path);

//    if (!data)
//        return false;
//    else if (share)
//        pResourcesData_[0] = data;
//    else
//        pResourcesData_[1] = data;

//    return true;
//}

//bool OGWorld::_LoadScene(const QString &path)
//{
//    logInfo("scene file");

//    if (pSceneData_)
//        return false;

//    pSceneData_ = OGData::GetScene(path);

//    return pSceneData_;
//}

//bool OGWorld::_LoadText(const QString &path, bool share)
//{
//    logInfo("text file");

//    WOGText* data;

//    if (share)
//        data = pTextData_[0];
//    else
//        data = pTextData_[1];

//    if (data)
//        return false;

//    OGTextConfig config(path);

//    bool status = true;

//    if (config.Open())
//    {
//        if (config.Read())
//        {
//            data = config.Parser(language_);

//            if (!data)
//                status = false;
//            else if (share)
//                pTextData_[0] = data;
//            else
//                pTextData_[1] = data;
//        }
//        else
//        {
//            logWarn("File " + path + " is corrupted");

//            status = false;
//        }
//    }
//    else
//    {
//        logWarn("File " + path + " not found");

//        status = false;
//    }

//    return status;
//}

//void OGWorld::_ClearLocalData()
//{
//    if (pLevelData_)
//    {
//        logInfo("Clear level");

//        delete pLevelData_;
//        pLevelData_ = 0;
//    }

//    if (pSceneData_)
//    {
//        logInfo("Clear scene");

//        delete pSceneData_;
//        pSceneData_ = 0;
//    }

//    if (pResourcesData_[1])
//    {
//        logInfo("Clear resources");

//        delete pResourcesData_[1];
//        pResourcesData_[1] = 0;
//    }

//    if (pTextData_[1])
//    {
//        logInfo("Clear text");

//        delete pTextData_[1];
//        pTextData_[1] = 0;
//    }
//}

//void OGWorld::CreateScene()
//{
//    logInfo("Create scene");

//    logInfo("Create scenelayers");

//    foreach (auto sceneLayer, scenedata()->sceneLayer)
//    {
//        _CreateSceneLayer(*sceneLayer, &sprites_);
//    }

//    if (!scenedata()->buttongroup.isEmpty())
//    {
//        logInfo("Create buttongroups");

//        foreach (const auto btnGroup, scenedata()->buttongroup)
//        {
//            foreach (const auto btn, btnGroup->button)
//            {
//                AddEntity(m_entityFactory.CreateButton(*btn));
//            }
//        }
//    }

//    if (!scenedata()->button.isEmpty())
//    {
//        logInfo("Create buttons");

////        foreach (auto btn, scenedata()->button)
////        {
////           // _CreateButton(*btn, &sprites_, &buttons_);
////        }
//    }

//    GameData gd;
//    gd.efactory = &m_entityFactory;
//    gd.pipe = GetPipeData();
//    gd.levelexit = leveldata()->levelexit;
//    gd.ballsrequired = leveldata()->ballsrequired;

//    AddEntity(m_entityFactory.CreateGameController(gd));

//    if (!pCamera_)
//    {
//        logInfo("Creating cameras");

//        if (!_CreateCamera())
//        {
//            logDebug("Error initializing camera");
//            return;
//        }
//    }

//    if (leveldata()->levelexit)
//    {
//        xExit_ = leveldata()->levelexit->pos.x() * 0.1;
//        yExit_ = leveldata()->levelexit->pos.y() * 0.1;
//    }

//    CreatePhysicsScene();

//    // Create Z-order
//    logInfo("Create Z-order");

////    _CreateZOrder();
//}

//OGIBody* OGWorld::AddStaticBody(OGIBody* a_body)
//{
//    assert(a_body);
//    staticBodies_.push_back(a_body);

//    return a_body;
//}

//void OGWorld::CreatePhysicsScene()
//{
//    logInfo("Creating physics");

//    foreach (auto circle, scenedata()->circle)
//    {
//        if (!circle->dynamic)
//            AddStaticBody(_CreateBody<OGCircle>(*circle));
//    }

//    foreach (auto rect, scenedata()->rectangle)
//    {
//        if (!rect->dynamic)
//            AddStaticBody(_CreateBody<OGRectangle>(*rect));
//    }

//    foreach (auto line, scenedata()->line)
//    {
//        if (!line->dynamic)
//            AddStaticBody(_CreateBody<OGLine>(*line));
//    }

//    foreach (auto cg, scenedata()->compositegeom)
//    {
//        _CreateCompositeGeom(cg);
//    }

//    if (!leveldata()->ball.empty())
//    {
//        logInfo("Creating balls");

//        QHash<QString, EntityPtr> balls;
//        foreach (auto ball, leveldata()->ball)
//        {
//            balls[ball->id] = AddEntity(m_entityFactory.CreateBall(*ball));
//        }

//        foreach (auto strand, leveldata()->strand)
//        {
//           auto e = m_entityFactory.CreateStrand(balls[strand->gb1], balls[strand->gb2]);
//           if (e)
//               AddEntity(e);
//        }
//    }

//    if (!scenedata()->radialforcefield.isEmpty())
//        logInfo("Creating radialforcefields");

//    foreach (auto ff, scenedata()->radialforcefield)
//    {
//        AddEntity(m_entityFactory.CreateRadialForceField(*ff));
//    }

//    _SetGravity();

//    m_physicsEngine->SetSimulation(6, 2, 60);

//    pNearestBall_ = 0;

//    isLevelLoaded_ = true;
//}

//void OGWorld::_CreateSceneLayer(const WOGSceneLayer &scenelayer, OGSpriteList* sprites)
//{
//    OGSprite* sprite = _CreateSprite(&scenelayer, scenelayer.image);
//    sprites->push_back(sprite);
//}

//void OGWorld::_CreateButton(const WOGButton &button, OGSpriteList* sprites, QList<OGButton*>* buttons)
//{
//    sprites->push_back(_CreateSprite(&button, button.up));
//    auto spr = sprites->last();

//    auto btn = new OGButton;
//    btn->setPosition(button.position);
//    btn->onclick(button.onclick);
//    btn->up(spr);
//    QSize sz(btn->up()->GetScaledWidth(), btn->up()->GetScaledHeight());
//    btn->setSize(sz);

//    sprites->push_back(_CreateSprite(&button, button.over));
//    spr = sprites->last();

//    btn->over(spr);
//    btn->over()->SetVisible(false);

//    if (!button.disabled.isEmpty())
//    {
//        sprites->push_back(_CreateSprite(&button, button.disabled));
//        spr = sprites->last();

//        btn->disabled(spr);
//        btn->disabled()->SetVisible(false);
//    }

//    buttons->push_back(btn);
//}

//bool OGWorld::_CreateCamera()
//{
//    float w = og::OGGameEngine::getInstance()->getWidth();
//    float h = og::OGGameEngine::getInstance()->getHeight();

//    if (w == 0)
//        return false;

//    float aspect = h / w;
//    WOGCamera* cam;

//    if (aspect == 0.75f)
//        cam =  pLevelData_->GetCameraByAspect("normal");
//    else
//        cam = pLevelData_->GetCameraByAspect("widescreen");

//    if (!cam || cam->poi.isEmpty())
//        return false;

//    Rect scene = RectF(pSceneData_->maxx, pSceneData_->maxy
//                       , pSceneData_->minx, pSceneData_->miny).ToRect();
//    Size size(w, h);

//    pCamera_ = new OGWindowCamera(scene, size, cam);

//    return true;
//}

//OGSprite* OGWorld::_CreateSprite(const WOGVObject* vobject, const QString &image)
//{
//    auto x = vobject->position.x();
//    auto y = -vobject->position.y();

//    auto src = GE->getResourceManager()->GetImageSourceById(image);
//    auto sprite = std::unique_ptr<OGSprite>(new OGSprite(x, y, src));
//    sprite->CenterOrigin();
//    sprite->SetScaleX(vobject->scale.x());
//    sprite->SetScaleY(vobject->scale.y());
//    sprite->SetAngle(-vobject->rotation);

//    sprite->SetColorize(vobject->colorize);
//    sprite->SetAlpha(vobject->alpha);
//    sprite->SetDepth(vobject->depth);

//    return sprite.release();
//}

//void OGWorld::_SetGravity()
//{
//    if (!scenedata()->linearforcefield.isEmpty())
//    {
//        if (scenedata()->linearforcefield.last()->type == "gravity")
//        {
//            float x = scenedata()->linearforcefield.last()->force.x();
//            float y = scenedata()->linearforcefield.last()->force.y();
//            GetPhysicsEngine().SetGravity(x, y);
//        }
//    }
//}

//void OGWorld::_ClearPhysics()
//{
//    if (_forceFilds.size())
//    {
//        logInfo("Clear force fields");
//        _forceFilds.clear();
//    }

//    logInfo("Clear strands");

//    QHashIterator<int, OGStrand*> i(strands_);

//    while (i.hasNext())
//    {
//        i.next();
//        delete i.value();
//    }

//    strands_.clear();

//    logInfo("Clear balls");

//    while (!balls_.empty())
//        delete balls_.takeFirst();

//    logInfo("Clear static bodies");

//    while (!staticBodies_.isEmpty())
//        delete staticBodies_.takeFirst();

//    // delete compositgeom
//    _cg.rectangle.clear();
//    _cg.circle.clear();

//    logInfo("Remove exit");

//    strandId_ = 0;
//    ballId_ = 0;

//    isLevelLoaded_ = false;
//}

//void OGWorld::_ClearScene()
//{
//    if (!sprites_.isEmpty())
//    {
//        logInfo("Clear sprites");

//        while (!sprites_.isEmpty())
//            delete sprites_.takeFirst();
//    }

//    if (!buttons_.isEmpty())
//    {
//        logInfo("Clear buttons");

//        while (!buttons_.isEmpty())
//            delete buttons_.takeFirst();
//    }

//    if (pCamera_)
//    {
//        logInfo("Clear camera");

//        delete pCamera_;
//        pCamera_ = 0;
//    }

//    m_update.clear();
//}

void OGWorld::CreateStrand(OGBall* /*b1*/, OGBall* /*b2*/)
{
//    if (b1->id() == -1)
//        b1->SetId(ballId_++);

//    if (b2->id() == -1)
//        b2->SetId(ballId_++);

//    strands_.insert(strandId_, new OGStrand(GetPhysicsEngine(), b1, b2, strandId_));
//    strandId_++;
}

//void OGWorld::findNearestAttachedBall()
//{
//    if (leveldata()->levelexit == 0)
//        return;

//    qreal dx, dy, x2, y2, length, tmpLength;
//    bool isInit = false;

//    Q_FOREACH(OGBall * ball, balls_)
//    {
//        if (ball->IsAttached())
//        {
//            if (!isInit)
//            {
//                pNearestBall_ = ball;
//                auto pos = pNearestBall_->GetPhyPosition();
//                x2 = pos.x();
//                y2 = pos.y();
//                dx = xExit_ - x2;
//                dy = yExit_ - y2;
//                length = dx * dx + dy * dy;
//                isInit = true;
//            }
//            else
//            {
//                auto pos = pNearestBall_->GetPhyPosition();
//                x2 = pos.x();
//                y2 = pos.y();
//                dx = xExit_ - x2;
//                dy = yExit_ - y2;
//                tmpLength = dx * dx + dy * dy;

//                if (tmpLength < length)
//                {
//                    length = tmpLength;
//                    pNearestBall_ = ball;
//                }
//            }
//        }
//    }
//}

void OGWorld::Update()
{
    if (m_physicsEngine)
        m_physicsEngine->Simulate();

    for (int i = 0; i < m_update.size(); ++i)
    {
        m_update[i]->Update();
    }

    if (!m_added[m_num_added].isEmpty())
    {
        auto& added = m_added[m_num_added];
        m_num_added = ++m_num_added % 2;

        for (int i = 0; i < added.size(); ++i)
        {
            m_update.push_back(std::move(added[i]));
//            m_update.back()->SetScene(this);
            m_update.back()->Added();
        }

        added.clear();

//        _CreateZOrder();
    }
}

void OGWorld::Render(QPainter& a_p)
{
    for (int i = 0; i < m_update.size(); ++i)
    {
        m_update[i]->Render(a_p);
    }
}

//MusicSPtr OGWorld::GetMusic(const QString& aId)
//{
//    return GE->getResourceManager()->GetMusic(aId);
//}

//bool OGWorld::LoadLevel()
//{
//    if (!Load())
//        return false;

//    CreateScene();

//    if (!isLevelLoaded())
//    {
//        CloseLevel();

//        return false;
//    }

//    auto& id = leveldata()->music.id;
//    auto bg = GetMusic(id);
//    bg->SetLoop(true);
//    bg->Play();

//    return true;
//}

//void OGWorld::CloseLevel()
//{
//    logInfo("[Close] Level");

//    logInfo("  [Clear] Scene");
//    _ClearScene();

//    logInfo("  [Clear] Physics");
//    _ClearPhysics();

//    logInfo("  [Clear] Local data");
//    _ClearLocalData();
//}

//void OGWorld::_CreateZOrder()
//{
//    foreach (OGSprite* sprite, sprites_)
//    {
//        _GetGame()->AddSprite(sprite->GetDepth(), sprite);
//    }
//}

//void OGWorld::_InsertSprite(OGSprite* sprite)
//{
//    assert(sprite);
//    sprites_.append(sprite);
//}

void OGWorld::RemoveStrand(OGStrand* /*strand*/)
{
//    delete strands_.take(strand->id());
}

//inline WOGPipe* OGWorld::GetPipeData()
//{
//    return pLevelData_->pipe;
//}

//inline OpenGOO* OGWorld::_GetGame()
//{
//    return OpenGOO::GetInstance();
//}

//void OGWorld::_CreateCompositeGeom(WOGCompositeGeom* cg)
//{
//    foreach(auto& circle, cg->circle)
//    {
//        staticBodies_.push_back(_CreateBody<OGCircle>(circle));
//    }

//    foreach(auto rect, cg->rectangle)
//    {
//        staticBodies_.push_back(_CreateBody<OGRectangle>(rect));
//    }
//}

EntityPtr OGWorld::AddEntity(EntityPtr a_e)
{
    m_added[m_num_added].push_back(a_e);

    return a_e;
}

void OGWorld::OnMouseDown(const QPoint& a_point)
{
    QVector2D point(a_point);
    for (auto it = m_update.cbegin(); it != m_update.cend(); ++it)
    {
        if (auto col = (*it)->GetCollider())
        {
            if (col->OverlapPoint(point))
            {
                (*it)->OnMouseDown();
                break;
            }
        }
    }
}

void OGWorld::OnMouseUp(const QPoint& a_point)
{
    QVector2D point(a_point);
    for (auto it = m_update.cbegin(); it != m_update.cend(); ++it)
    {
        if (auto col = (*it)->GetCollider())
        {
            if (col->OverlapPoint(point))
            {
                (*it)->OnMouseUp();
                break;
            }
        }
    }
}

void OGWorld::OnMouseMove(const QPoint& a_point)
{
    QVector2D point(a_point);
    for (auto it = m_update.cbegin(); it != m_update.cend(); ++it)
    {
        if (auto col = (*it)->GetCollider())
        {
            if (col->OverlapPoint(point))
            {
//                FIXME no member named 'OnMouseMove' in 'og::Entity'
//                (*it)->OnMouseMove();
                break;
            }
        }
    }
}
