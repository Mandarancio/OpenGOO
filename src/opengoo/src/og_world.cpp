#include <memory>

#include <QPainter>
#include <QFile>
#include <QTimer>

#include "og_world.h"
#include "logger.h"
#include "og_data.h"
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

#include "gamedata.h"
#include "entityfactory.h"

using namespace og;

OGWorld::OGWorld(EntityFactory& a_factory,
                 const QString &levelname,
                 QObject* parent)
    : QObject(parent),
      m_factory(a_factory)
{
    levelName_ = levelname;

    pLevelData_ = 0;
    pSceneData_ = 0;
    pResourcesData_[0] = 0;  // global resources
    pResourcesData_[1] = 0;  // local resources
    pTextData_[0] = 0;       // global text
    pTextData_[1] = 0;       // local text
    pMaterialData_ = 0;
    pEffectsData_ = 0;
    pTimer_ = 0;
    pCamera_ = 0;
    pNearestBall_ = 0;
    pPhysicsEngine_ = 0;

    strandId_ = 0;
    ballId_ = 0;

    isPhysicsEngine_ = false;
    isLevelLoaded_ = false;
    m_num_added = 0;
}

OGWorld::~OGWorld()
{
    _ClearLocalData(); // clear local data

    logInfo("Clear share data");

    if (pResourcesData_[0])
        delete pResourcesData_[0];

    if (pTextData_[0])
        delete pTextData_[0];

    if (pMaterialData_)
        delete pMaterialData_;

    if (pEffectsData_)
        delete pEffectsData_;

    delete pTimer_;

    logInfo("Destroy physics engine");

    og::physics::PhysicsEngine::DestroyInstance();
}

bool OGWorld::isExist(const QString &path_level)
{
    QString path;

    path = "./res/levels/" + path_level + "/" + path_level + ".level";

    if (!QFile::exists(path + ".xml") && !QFile::exists(path + ".bin"))
    {
        logDebug(QString("File %1 not found").arg(path));
        return false;
    }

    path = "./res/levels/" + path_level + "/" + path_level  + ".scene";

    if (!QFile::exists(path + ".xml") && !QFile::exists(path + ".bin"))
    {
        logDebug(QString("File %1 not found").arg(path));
        return false;
    }

    path = "./res/levels/" + path_level + "/" + path_level  + ".resrc";

    if (!QFile::exists(path + ".xml") && !QFile::exists(path + ".bin"))
    {
        logDebug(QString("File %1 not found").arg(path));
        return false;
    }

    return true;
}

bool OGWorld::Initialize()
{
    logInfo("Initializing the physics engine");

    if (!_InitializePhysics())
    {
        logError("Unable init physics engine.");

        return false;
    }

    logInfo("Loading share data...");

    QString path = "./properties/materials.xml";

    _LoadMaterials(path);

    path = "./properties/resources.xml";

    if (!_LoadResources(path, true))
        return false;

    path = "./properties/fx.xml";

    _LoadFX(path);

    path = "./properties/text.xml";

    if (!_LoadText(path, true))
        return false;

    return true;
}

bool OGWorld::Load()
{
    logInfo("Loading ...");

    if (isExist(levelName_))
    {
        QString path = "./res/levels/" + levelName_ + "/" + levelName_ + ".level";

        if (!_LoadLevel(path))
            return false;

        path = "./res/levels/" + levelName_ + "/" + levelName_  + ".scene";

        if (!_LoadScene(path))
            return false;

        path = "./res/levels/" + levelName_ + "/" + levelName_  + ".resrc";

        if (!_LoadResources(path, false))
            return false;
    }
    else
    {
        logWarn(QString("Wrong level name: \"%1\"").arg(levelName_));

        return false;
    }

    return true;
}

void OGWorld::Reload()
{
    _ClearPhysics();
    CreatePhysicsScene();
}

template<class Target, class Config>
Target OGWorld::LoadConf(const QString &path)
{
    Config config(path);

    if (config.Open())
    {
        if (config.Read())
            return config.Parser();
        else
            logWarn("File " + path + " is corrupted");
    }
    else
        logWarn("File " + path + " not found");

    return 0;
}

bool OGWorld::_LoadFX(const QString &path)
{
    logInfo("effects file");

    if (pEffectsData_)
        return false;

    pEffectsData_ = LoadConf<WOGEffects*, OGEffectConfig> (path);

    if (pEffectsData_)
        return true;
    else
        return false;
}

bool OGWorld::_LoadLevel(const QString path)
{
    logInfo("level file");

    if (pLevelData_)
        return false;

    pLevelData_ = LoadConf<WOGLevel*, OGLevelConfig> (path);

    if (pLevelData_)
        return true;
    else
        return false;
}

bool OGWorld::_LoadMaterials(const QString &path)
{
    logInfo("material file");

    if (pMaterialData_)
        return false;

    pMaterialData_ = LoadConf<WOGMaterialList*, OGMaterialConfig> (path);

    if (pMaterialData_)
        return true;
    else
        return false;
}

bool OGWorld::_LoadResources(const QString &path, bool share)
{
    logInfo("resources file");

    WOGResources* data;

    if (share)
        data = pResourcesData_[0];
    else
        data = pResourcesData_[1];

    if (data)
        return false;

    if (share)
        data = OGData::GetResources(path);
    else
        data = OGData::GetResources(path);

    if (!data)
        return false;
    else if (share)
        pResourcesData_[0] = data;
    else
        pResourcesData_[1] = data;

    return true;
}

bool OGWorld::_LoadScene(const QString &path)
{
    logInfo("scene file");

    if (pSceneData_)
        return false;

    pSceneData_ = OGData::GetScene(path);

    if (pSceneData_)
        return true;
    else
        return false;
}

bool OGWorld::_LoadText(const QString &path, bool share)
{
    logInfo("text file");

    WOGText* data;

    if (share)
        data = pTextData_[0];
    else
        data = pTextData_[1];

    if (data)
        return false;

    OGTextConfig config(path);

    bool status = true;

    if (config.Open())
    {
        if (config.Read())
        {
            data = config.Parser(language_);

            if (!data)
                status = false;
            else if (share)
                pTextData_[0] = data;
            else
                pTextData_[1] = data;
        }
        else
        {
            logWarn("File " + path + " is corrupted");

            status = false;
        }
    }
    else
    {
        logWarn("File " + path + " not found");

        status = false;
    }

    return status;
}

void OGWorld::_ClearLocalData()
{
    if (pLevelData_)
    {
        logInfo("Clear level");

        delete pLevelData_;
        pLevelData_ = 0;
    }

    if (pSceneData_)
    {
        logInfo("Clear scene");

        delete pSceneData_;
        pSceneData_ = 0;
    }

    if (pResourcesData_[1])
    {
        logInfo("Clear resources");

        delete pResourcesData_[1];
        pResourcesData_[1] = 0;
    }

    if (pTextData_[1])
    {
        logInfo("Clear text");

        delete pTextData_[1];
        pTextData_[1] = 0;
    }
}

og::physics::PhysicsEngine* OGWorld::GetPhisicsEngine()
{
    if (!pPhysicsEngine_)
        pPhysicsEngine_ = PE;

    return pPhysicsEngine_;
}

void OGWorld::CreateScene()
{
    logInfo("Creating physics");
    GetPhisicsEngine();

    logInfo("Create scene");

    logInfo("Create scenelayers");

    foreach (auto sceneLayer, scenedata()->sceneLayer)
    {
        _CreateSceneLayer(*sceneLayer, &sprites_);
    }

    logInfo("Create buttongroups");

    foreach (auto btnGroup, scenedata()->buttongroup)
    {
        foreach (auto btn, btnGroup->button)
        {
            _CreateButton(*btn, &sprites_ , &buttons_);
        }
    }

    logInfo("Create buttons");

    foreach (auto btn, scenedata()->button)
    {
        _CreateButton(*btn, &sprites_, &buttons_);
    }

    GameData gd;
    gd.efactory = &m_factory;
    gd.pipe = GetPipeData();
    gd.levelexit = leveldata()->levelexit;
    gd.ballsrequired = leveldata()->ballsrequired;

    AddEntity(m_factory.CreateGameController(gd));

    if (!pCamera_)
    {
        logInfo("Creating cameras");

        if (!_CreateCamera())
        {
            logDebug("Error initializing camera");
            return;
        }
    }

    if (leveldata()->levelexit != 0)
    {
        xExit_ = leveldata()->levelexit->pos.x() * 0.1;
        yExit_ = leveldata()->levelexit->pos.y() * 0.1;
    }

    CreatePhysicsScene();

    // Create Z-order
    logInfo("Create Z-order");

    _CreateZOrder();
}

OGIBody* OGWorld::AddStaticBody(OGIBody* a_body)
{
    assert(a_body);
    staticBodies_.push_back(a_body);

    return a_body;
}

void OGWorld::CreatePhysicsScene()
{
    logInfo("Creating physics");

    foreach (auto circle, scenedata()->circle)
    {
        if (!circle->dynamic)
            AddStaticBody(_CreateBody<OGCircle, WOGCircle>(circle));
    }

    foreach (auto rect, scenedata()->rectangle)
    {
        if (!rect->dynamic)
            AddStaticBody(_CreateBody<OGRectangle, WOGRectangle>(rect));
    }

    foreach (auto line, scenedata()->line)
    {
        if (!line->dynamic)
            AddStaticBody(_CreateBody<OGLine, WOGLine>(line));
    }

    foreach (auto cg, scenedata()->compositegeom)
    {
        _CreateCompositeGeom(cg);
    }

    foreach (auto ball, leveldata()->ball)
    {
        balls_.push_back(_CreateBall(ball));
    }

    foreach (auto strand, leveldata()->strand)
    {
        _CreateStrand(strand);
    }

    foreach (auto ff, scenedata()->radialforcefield)
    {
        _forceFilds.push_back(std::move(_CreateRadialForcefield(ff)));
    }

    _SetGravity();

    pPhysicsEngine_->SetSimulation(6, 2, 60);

    pNearestBall_ = 0;

    if (pTimer_ == 0)
    {
        pTimer_ = new QTimer(this);
        connect(pTimer_, SIGNAL(timeout()), this, SLOT(findNearestAttachedBall()));
        pTimer_->start(1000);
    }

    isLevelLoaded_ = true;    
}

void OGWorld::_CreateSceneLayer(const WOGSceneLayer &scenelayer
                                , OGSpriteList* sprites)
{
    OGSprite* sprite = _CreateSprite(&scenelayer, scenelayer.image);
    sprites->push_back(sprite);
}

OGBall* OGWorld::_CreateBall(WOGBallInstance* ball)
{
    OGBall* obj = 0;
    WOGBall* configuration = GetBallConfiguration(ball->type);

    if (configuration)
    {
        if (configuration->attribute.core.shape->type == "circle")
            obj = new OGBall(ball, configuration);
        else if (configuration->attribute.core.shape->type == "rectangle")
            obj = new OGBall(ball, configuration);
    }

    return obj;
}

template<class Body, class Data> Body* OGWorld::_CreateBody(Data* data)
{
    Body* obj = 0;
    QString id = data->material;
    WOGMaterial* material = materialdata()->GetMaterial(id);

    if (material)
        obj = new Body(data, material);
    else
        logError(QString("Wrong material id: %1").arg(id));

    return obj;
}

void OGWorld::_CreateButton(const WOGButton &button,
                            OGSpriteList* sprites,
                            QList<OGButton*>* buttons)
{
    sprites->push_back(_CreateSprite(&button, button.up));
    auto spr = sprites->last();

    auto btn = new OGButton;
    btn->setPosition(button.position);
    btn->onclick(button.onclick);
    btn->up(spr);
    QSize sz(btn->up()->GetScaledWidth(), btn->up()->GetScaledHeight());
    btn->setSize(sz);

    sprites->push_back(_CreateSprite(&button, button.over));
    spr = sprites->last();

    btn->over(spr);
    btn->over()->SetVisible(false);

    if (!button.disabled.isEmpty())
    {
        sprites->push_back(_CreateSprite(&button, button.disabled));
        spr = sprites->last();

        btn->disabled(spr);
        btn->disabled()->SetVisible(false);
    }

    buttons->push_back(btn);
}

bool OGWorld::_CreateCamera()
{
    float w = og::OGGameEngine::getInstance()->getWidth();
    float h = og::OGGameEngine::getInstance()->getHeight();

    if (w == 0)
        return false;

    float aspect = h / w;
    WOGCamera* cam;

    if (aspect == 0.75f)
        cam =  pLevelData_->GetCameraByAspect("normal");
    else
        cam = pLevelData_->GetCameraByAspect("widescreen");

    if (!cam || cam->poi.isEmpty())
        return false;

    Rect scene = RectF(pSceneData_->maxx, pSceneData_->maxy
                       , pSceneData_->minx, pSceneData_->miny).ToRect();
    Size size(w, h);

    pCamera_ = new OGWindowCamera(scene, size, cam);

    return true;
}

ImageSourcePtr OGWorld::CreateImageSource(const QString& a_id)
{
    auto file = pResourcesData_[1]->GetResource(WOGResource::IMAGE,
                                                a_id) + ".png";
    return std::make_shared<ImageSource>(file);
}

OGSprite* OGWorld::_CreateSprite(const WOGVObject* vobject
                                 , const QString &image)
{
    auto x = vobject->position.x();
    auto y = -vobject->position.y();

    auto src = CreateImageSource(image);
    auto sprite = std::unique_ptr<OGSprite>(new OGSprite(x, y, src));
    sprite->SetScaleX(vobject->scale.x());
    sprite->SetScaleY(vobject->scale.y());
    sprite->SetAngle(vobject->rotation);
    sprite->CenterOrigin();

    sprite->SetColorize(vobject->colorize);
    sprite->SetAlpha(vobject->alpha);
    sprite->SetDepth(vobject->depth);

    return sprite.release();
}

void OGWorld::_CreateStrand(WOGStrand* strand)
{
    OGBall* b1 = 0;
    OGBall* b2 = 0;

    Q_FOREACH(OGBall * ball, balls_)
    {
        if (strand->gb1 == ball->GetId())
            b1 = ball;
        else if (strand->gb2 == ball->GetId())
            b2 = ball;

        if (b1 && b2)
        {
            b1->Attache(b2);

            if (b1->id() == -1)
                b1->SetId(ballId_++);
            if (b2->id() == -1)
                b2->SetId(ballId_++);

            break;
        }
    }
}

WOGBall* OGWorld::GetBallConfiguration(const QString &type)
{
    WOGBall* configuration = ballConfigurations_.object(type);

    if (!configuration)
    {
        QString path = "./res/balls/" + type + "/balls.xml";

        configuration = LoadConf<WOGBall*, OGBallConfig> (path);

        if (configuration)
            ballConfigurations_.insert(type, configuration);
    }

    return  configuration;
}

bool OGWorld::_InitializePhysics()
{
    return PE->Initialize(0, -10, true);
}

void OGWorld::_SetGravity()
{
    if (!scenedata()->linearforcefield.isEmpty())
    {
        if (scenedata()->linearforcefield.last()->type == "gravity")
        {
            float x = scenedata()->linearforcefield.last()->force.x();
            float y = scenedata()->linearforcefield.last()->force.y();
            PE->SetGravity(x, y);
        }
    }
}


void OGWorld::_ClearPhysics()
{
    pPhysicsEngine_ = 0;

    if (_forceFilds.size())
    {
        logInfo("Clear force fields");
        _forceFilds.clear();
    }

    logInfo("Clear strands");

    QHashIterator<int, OGStrand*> i(strands_);

    while (i.hasNext())
    {
        i.next();
        delete i.value();
    }

    if (pTimer_)
    {
        delete pTimer_;
        pTimer_ = 0;
    }

    strands_.clear();

    logInfo("Clear balls");

    while (!balls_.empty())
        delete balls_.takeFirst();

    logInfo("Clear static bodies");

    while (!staticBodies_.isEmpty())
        delete staticBodies_.takeFirst();

    // delete compositgeom
    while (!_cg.rectangle.isEmpty())
        delete _cg.rectangle.takeFirst();

    while (!_cg.circle.isEmpty())
        delete _cg.circle.takeFirst();

    logInfo("Remove exit");

    strandId_ = 0;
    ballId_ = 0;

    PE->Reload();

    isLevelLoaded_ = false;
}

void OGWorld::_ClearScene()
{
    if (!sprites_.isEmpty())
    {
        logInfo("Clear sprites");

        while (!sprites_.isEmpty())
            delete sprites_.takeFirst();
    }

    if (!buttons_.isEmpty())
    {
        logInfo("Clear buttons");

        while (!buttons_.isEmpty())
            delete buttons_.takeFirst();
    }

    if (pCamera_)
    {
        logInfo("Clear camera");

        delete pCamera_;
        pCamera_ = 0;
    }

    m_update.clear();
}

void OGWorld::CreateStrand(OGBall* b1, OGBall* b2)
{
    if (b1->id() == -1)
        b1->SetId(ballId_++);

    if (b2->id() == -1)
        b2->SetId(ballId_++);

    OGStrand* obj = new OGStrand(b1, b2, strandId_);
    strands_.insert(strandId_, obj);
    strandId_++;
}

void OGWorld::findNearestAttachedBall()
{
    if (leveldata()->levelexit == 0)
        return;

    qreal dx, dy, x2, y2, length, tmpLength;
    bool isInit = false;

    Q_FOREACH(OGBall * ball, balls_)
    {
        if (ball->IsAttached())
        {
            if (!isInit)
            {
                pNearestBall_ = ball;
                x2 = pNearestBall_->GetX();
                y2 = pNearestBall_->GetY();
                dx = xExit_ - x2;
                dy = yExit_ - y2;
                length = dx * dx + dy * dy;
                isInit = true;
            }
            else
            {
                x2 = ball->GetX();
                y2 = ball->GetY();
                dx = xExit_ - x2;
                dy = yExit_ - y2;
                tmpLength = dx * dx + dy * dy;

                if (tmpLength < length)
                {
                    length = tmpLength;
                    pNearestBall_ = ball;
                }
            }
        }
    }
}

void OGWorld::Update()
{
    if (pPhysicsEngine_)
        pPhysicsEngine_->Simulate();

    for (int i = 0; i < m_update.size(); ++i)
    {
        m_update[i]->Update();
    }

    if (!m_added[m_num_added].isEmpty())
    {
        auto& added = m_added[m_num_added];
        m_num_added = ++m_num_added % 2;

        for (int i = 0;  i < added.size(); ++i)
        {
            m_update.push_back(std::move(added[i]));
            m_update.back()->SetScene(this);
            m_update.back()->Added();
        }

        added.clear();

        _CreateZOrder();
    }
}

void OGWorld::Render(QPainter& a_p)
{
    for (int i = 0; i < m_update.size(); ++i)
    {
        m_update[i]->Render(a_p);
    }
}

bool OGWorld::LoadLevel(const QString &levelname)
{
    SetLevelname(levelname);

    if (!Load())
        return false;

    CreateScene();

    if (!isLevelLoaded())
    {
        CloseLevel();

        return false;
    }

    return true;
}

void OGWorld::CloseLevel()
{
    logInfo("Close level");

    _ClearPhysics();
    _ClearScene();
    _ClearLocalData();
}

void OGWorld::_CreateZOrder()
{
#define NEW_ZORDER

#ifndef NEW_ZORDER
//    // Bubble sort
//    // Source http://en.wikibooks.org/wiki/Algorithm_Implementation/Sorting/Bubble_sort#C.2B.2B
    QList <OGSprite*>::iterator first = sprites_.begin();
    QList <OGSprite*>::iterator last = sprites_.end();
    QList <OGSprite*>::iterator i;

    while (first < --last)
    {
        for (i = first; i < last; ++i)
        {
            if ((*(i + 1))->depth < (*i)->depth)
            {
                std::iter_swap(i, i + 1);
            }
        }
    }

#else
    foreach (OGSprite* sprite, sprites_)
    {
        _GetGame()->AddSprite(sprite->GetDepth(), sprite);
    }
#endif
}

void OGWorld::_InsertSprite(OGSprite* sprite)
{
    assert(sprite);
    sprites_.append(sprite);
}

void OGWorld::RemoveStrand(OGStrand* strand)
{
    delete strands_.take(strand->id());
}

inline void  OGWorld::StartSearching()
{
    pTimer_->start(1000);
}

inline WOGPipe* OGWorld::GetPipeData()
{
    return pLevelData_->pipe;
}

inline OpenGOO* OGWorld::_GetGame()
{
    return OpenGOO::GetInstance();
}

RadialForceFieldPtr OGWorld::_CreateRadialForcefield(WOGRadialForceField* ff)
{
    float x = ff->center.x() / 10;
    float y = ff->center.y() / 10;
    float r = ff->radius / 10;
    Circle c(QVector2D(x, y), r);
    RadialForceFieldPtr rf(new RadialForceField(Circle(c)));
    rf->setForceatCenter(ff->forceatcenter);
    rf->setForceatEdge(ff->forceatedge);

    return rf;
}

void OGWorld::_CreateCompositeGeom(WOGCompositeGeom* cg)
{
    foreach(auto circle, cg->circle)
    {
        auto wc = new WOGCircle;
        *wc = *circle;
        wc->material = cg->material;
        wc->tag = cg->tag;
        wc->position += cg->position;
        wc->dynamic = cg->dynamic;
        _cg.circle << wc;

        if (!wc->dynamic)
            staticBodies_ << _CreateBody<OGCircle, WOGCircle>(wc);
    }


    foreach(auto rect, cg->rectangle)
    {
        auto wr = new WOGRectangle;
        *wr = *rect;
        wr->material = cg->material;;
        wr->tag = cg->tag;
        wr->position += cg->position;
        wr->dynamic = cg->dynamic;
        _cg.rectangle << wr;

        if (!wr->dynamic)
            staticBodies_ << _CreateBody<OGRectangle, WOGRectangle>(wr);
    }
}

EntityPtr OGWorld::AddEntity(EntityPtr a_e)
{
    m_added[m_num_added].push_back(a_e);

    return a_e;
}
