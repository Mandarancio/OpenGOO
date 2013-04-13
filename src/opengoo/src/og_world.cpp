#include "og_gameengine.h"
#include "og_world.h"
#include "logger.h"
#include "og_data.h"
#include "og_windowcamera.h"
#include "OGLib/rect.h"
#include "OGLib/rectf.h"
#include "OGLib/size.h"
#include "og_pipe.h"

#include <QPainter>
#include <QFile>

#include <memory>

OGWorld::OGWorld(const QString &levelname, QObject* parent)
    : QObject(parent)
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
    pPipe_ = 0;
    pNearestBall_ = 0;
    pPhysicsEngine_ = 0;

    strandId_ = 0;
    ballId_ = 0;

    isPhysicsEngine_ = false;
    isLevelLoaded_ = false;
}

OGWorld::~OGWorld()
{
    _ClearLocalData(); // clear local data

    logDebug("Clear share data");

    if (pResourcesData_[0]) { delete pResourcesData_[0]; }

    if (pTextData_[0]) { delete pTextData_[0]; }

    if (pMaterialData_) { delete pMaterialData_; }

    if (pEffectsData_) { delete pEffectsData_; }

    delete pTimer_;

    logDebug("Destroy physics engine");

    OGPhysicsEngine::DestroyInstance();
}

bool OGWorld::isExist(const QString &path_level)
{
    QString path;

    path = "./res/levels/" + path_level + "/" + path_level + ".level.xml";

    if (!QFile::exists(path)) { logDebug(QString("File %1 not found").arg(path)); return false; }

    path = "./res/levels/" + path_level + "/" + path_level  + ".scene.xml";

    if (!QFile::exists(path)) { logDebug(QString("File %1 not found").arg(path)); return false; }

    path = "./res/levels/" + path_level + "/" + path_level  + ".resrc.xml";

    if (!QFile::exists(path)) { logDebug(QString("File %1 not found").arg(path)); return false; }

    path = "./res/levels/" + path_level + "/" + path_level  + ".text.xml";

    if (!QFile::exists(path)) { logDebug(QString("File %1 not found").arg(path));}

    return true;
}

bool OGWorld::Initialize()
{
    logDebug("Initializing the physics engine");

    if (!_InitializePhysics())
    {
        logError("Unable init physics engine.");

        return false;
    }

    logDebug("Loading share data...");

    QString path;

    path = "./properties/materials.xml";

    _LoadMaterials(path);

    path = "./properties/resources.xml";

    if (!_LoadResources(path, true)) { return false; }

    path = "./properties/fx.xml";

    _LoadFX(path);

    path = "./properties/text.xml";

    if (!_LoadText(path, true)) { return false; }

    return true;
}

bool OGWorld::Load()
{
    logDebug("Loading ...");

    if (isExist(levelName_))
    {
        QString path;

        path = "./res/levels/" + levelName_ + "/" + levelName_ + ".level.xml";
        if (!_LoadLevel(path)) { return false; }

        path = "./res/levels/" + levelName_ + "/" + levelName_  + ".scene.xml";
        if (!_LoadScene(path)) { return false; }

        path = "./res/levels/" + levelName_ + "/" + levelName_  + ".resrc.xml";
        if (!_LoadResources(path, false)) { return false; }

        path = "./res/levels/" + levelName_ + "/" + levelName_  + ".text.xml";
        _LoadText(path, false);
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
        if (config.Read()) { return config.Parser(); }
        else { logWarn("File " + path + " is corrupted"); }
    }
    else { logWarn("File " + path + " not found"); }

    return 0;
}

bool OGWorld::_LoadFX(const QString &path)
{
    logDebug("effects file");

    if (pEffectsData_) { return false; }

    pEffectsData_ = LoadConf<WOGEffects*, OGEffectConfig> (path);

    if (pEffectsData_) { return true; }
    else { return false; }
}

bool OGWorld::_LoadLevel(const QString path)
{
    logDebug("level file");

    if (pLevelData_) { return false; }

    pLevelData_ = LoadConf<WOGLevel*, OGLevelConfig> (path);

    if (pLevelData_) { return true; }
    else { return false; }
}

bool OGWorld::_LoadMaterials(const QString &path)
{
    logDebug("material file");

    if (pMaterialData_) { return false; }

    pMaterialData_ = LoadConf<WOGMaterialList*, OGMaterialConfig> (path);

    if (pMaterialData_) { return true; }
    else { return false; }
}

bool OGWorld::_LoadResources(const QString &path, bool share)
{
    logDebug("resources file");

    WOGResources* data;

    if (share) data = pResourcesData_[0];
    else data = pResourcesData_[1];

    if (data) { return false; }

    if (share) { data = OGData::GetResources(path); }
    else { data = OGData::GetResources(path); }

    if (!data) {  return false; }
    else if (share) { pResourcesData_[0] = data; }
    else { pResourcesData_[1] = data; }

    return true;
}

bool OGWorld::_LoadScene(const QString &path)
{
    logDebug("scene file");

    if (pSceneData_) { return false; }

    pSceneData_ = OGData::GetScene(path);

    if (pSceneData_) { return true; }
    else { return false; }
}

bool OGWorld::_LoadText(const QString &path, bool share)
{
    logDebug("text file");

    WOGText* data;

    if (share) data = pTextData_[0];
    else data = pTextData_[1];

    if (data) { return false; }

    OGTextConfig config(path);

    bool status = true;

    if (config.Open())
    {
        if (config.Read())
        {
            data = config.Parser(language_);

            if (!data) { status = false; }
            else if (share) { pTextData_[0] = data; }
            else { pTextData_[1] = data; }
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
        logDebug("Clear level");

        delete pLevelData_;
        pLevelData_ = 0;
    }

    if (pSceneData_)
    {
        logDebug("Clear scene");

        delete pSceneData_;
        pSceneData_ = 0;
    }

    if (pResourcesData_[1])
    {
        logDebug("Clear resources");

        delete pResourcesData_[1];
        pResourcesData_[1] = 0;
    }

    if (pTextData_[1])
    {
        logDebug("Clear text");

        delete pTextData_[1];
        pTextData_[1] = 0;
    }
}

void OGWorld::CreateScene()
{
    logDebug("Create scene");

    logDebug("Create scenelayers");

    Q_FOREACH(WOGSceneLayer * sceneLayer, scenedata()->sceneLayer)
    {
        _CreateSceneLayer(*sceneLayer, &sprites_);
    }

    logDebug("Create buttongroups");

    Q_FOREACH(WOGButtonGroup * btnGroup, scenedata()->buttongroup)
    {
        Q_FOREACH(WOGButton * btn, btnGroup->button)
        {
            _CreateButton(*btn, &sprites_ , &buttons_);
        }
    }

    logDebug("Create buttons");

    Q_FOREACH(WOGButton * btn, scenedata()->button)
    {
        _CreateButton(*btn, &sprites_, &buttons_);
    }

    if (!pPipe_) _CreatePipe();

    // Create Z-order
    logDebug("Create Z-order");

    _CreateZOrder();

    if (!pCamera_)
    {
        logDebug("Creating cameras");

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
}

void OGWorld::CreatePhysicsScene()
{
    Q_FOREACH(WOGCircle * circle, scenedata()->circle)
    {
        if (!circle->dynamic)
        {
            staticBodies_ << _CreateBody<OGCircle, WOGCircle>(circle);
        }
    }

    Q_FOREACH(WOGRectangle * rect, scenedata()->rectangle)
    {
        if (!rect->dynamic)
        {
            staticBodies_ << _CreateBody<OGRectangle, WOGRectangle>(rect);
        }
    }

    Q_FOREACH(WOGLine * line, scenedata()->line)
    {
        if (!line->dynamic)
        {
            staticBodies_ << _CreateBody<OGLine, WOGLine>(line);
        }
    }

    Q_FOREACH(WOGBallInstance * ball, leveldata()->ball)
    {
        balls_ << _CreateBall(ball);
    }

    Q_FOREACH(WOGStrand * strand, leveldata()->strand)
    {
        _CreateStrand(strand);
    }

    _SetGravity();

    pPhysicsEngine_ = OGPhysicsEngine::GetInstance();

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
                                , QList<OGSprite*>* sprites)
{
    OGSprite* sprite = _CreateSprite(&scenelayer, scenelayer.image);

    sprite->visible = true;
    sprites->push_back(sprite);
}

OGBall* OGWorld::_CreateBall(WOGBallInstance* ball)
{
    OGBall* obj = 0;
    WOGBall* configuration = GetBallConfiguration(ball->type);

    if (configuration)
    {
        if (configuration->attribute.core.shape->type == "circle")
        {
            obj = new OGBall(ball, configuration);
        }
        else if (configuration->attribute.core.shape->type == "rectangle")
        {
            obj = new OGBall(ball, configuration);
        }
    }

    return obj;
}

template<class Body, class Data> Body* OGWorld::_CreateBody(Data* data)
{
    Body* obj = 0;
    QString id = data->material;
    WOGMaterial* material = materialdata()->GetMaterial(id);

    if (material) { obj = new Body(data, material); }
    else { logError(QString("Wrong material id: %1").arg(id)); }

    return obj;
}

void OGWorld::_CreateButton(const WOGButton &button, QList<OGSprite*>* sprites
                            , QList<OGButton*> *buttons)
{
    OGButton* btn;

    btn = new OGButton;

    sprites->push_back(_CreateSprite(&button, button.up));
    btn->position(button.position);
    btn->onclick(button.onclick);
    btn->up(sprites->last());
    btn->size(btn->up()->size);
    btn->up()->visible = true;

    sprites->push_back(_CreateSprite(&button, button.over));
    btn->over(sprites->last());
    btn->over()->visible = false;

    if (!button.disabled.isEmpty())
    {
        sprites->push_back(_CreateSprite(&button, button.disabled));
        btn->disabled(sprites->last());
        btn->disabled()->visible = false;
    }

    buttons->push_back(btn);
}

bool OGWorld::_CreateCamera()
{
    float w = OGGameEngine::getEngine()->getWidth();
    float h = OGGameEngine::getEngine()->getHeight();

    if (w == 0) return false;

    float aspect = h / w;
    WOGCamera* cam;

    if (aspect == 0.75f) cam =  pLevelData_->GetCameraByAspect("normal");
    else cam = pLevelData_->GetCameraByAspect("widescreen");

    if (!cam || cam->poi.isEmpty()) return false;

    Rect scene = RectF(pSceneData_->maxx, pSceneData_->maxy
                       , pSceneData_->minx, pSceneData_->miny).ToRect();
    Size size(w, h);


    pCamera_ = new OGWindowCamera(scene, size, cam);

    return true;
}

void OGWorld::_CreatePipe()
{
    WOGPipe* pipe = _GetPipeData();

    if (pipe)
    {
        logDebug("Create pipe");

        pPipe_ = new OGPipe(pipe);
    }
}

QPixmap OGWorld::_CreatePixmap(OGSprite* sprite, const QString &image)
{
    QImage source(pResourcesData_[1]->GetResource(WOGResource::IMAGE
                  , image) + ".png");

    QTransform transform;

    transform.rotate((-1.0)*sprite->rotation);
    QImage tmpImage(source.transformed(transform, Qt::SmoothTransformation));
    QImage target(tmpImage.size(), QImage::Format_ARGB32_Premultiplied);

    // Set the size of sprite
    transform.scale(sprite->scale.x(), sprite->scale.y());
    QImage tmpImage2 =
        QImage(source.size(), QImage::Format_Mono).transformed(transform);

    sprite->size = tmpImage2.size();

    // Colorize sprite
    QPainter painter(&target);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(target.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(target.rect(), tmpImage, tmpImage.rect());

    if (!source.hasAlphaChannel())
    {
        painter.setCompositionMode(QPainter::CompositionMode_Multiply);
        painter.fillRect(target.rect(), sprite->colorize);
    }
    else
    {
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(target.rect(), sprite->colorize);
        painter.setCompositionMode(QPainter::CompositionMode_Multiply);
        painter.drawImage(target.rect(), tmpImage, tmpImage.rect());
    }

    painter.end();

    return QPixmap::fromImage(target);
}

OGSprite* OGWorld::_CreateSprite(const WOGVObject* vobject
                                 , const QString &image)
{
    qreal x, y;
    OGSprite* sprite;

    sprite = new OGSprite;
    sprite->scale = vobject->scale;
    sprite->rotation = vobject->rotation;
    sprite->colorize = vobject->colorize;
    sprite->alpha = vobject->alpha;
    sprite->depth = vobject->depth;
    sprite->image = _CreatePixmap(sprite, image);

    // Set sprite position
    x = vobject->position.x() - sprite->size.width() * 0.5;
    y = (vobject->position.y() + sprite->size.height() * 0.5) * (-1.0);
    sprite->position = QPointF(x, y);

    return sprite;
}

void OGWorld::_CreateStrand(WOGStrand* strand)
{
    OGBall* b1 = 0;
    OGBall* b2 = 0;

    Q_FOREACH(OGBall * ball, balls_)
    {
        if (strand->gb1 == ball->GetId()) { b1 = ball; }
        else if (strand->gb2 == ball->GetId()) { b2 = ball; }

        if (b1 && b2)
        {
            b1->Attache(b2);

            if (b1->id() == -1) { b1->SetId(ballId_++); }
            if (b2->id() == -1) { b2->SetId(ballId_++); }

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

        if (configuration) { ballConfigurations_.insert(type, configuration); }
    }

    return  configuration;
}

bool OGWorld::_InitializePhysics()
{
    return OGPhysicsEngine::GetInstance()->Initialize(0, -10, true);
}

void OGWorld::_SetGravity()
{
    if (!scenedata()->linearforcefield.isEmpty())
    {
        if (scenedata()->linearforcefield.last()->type == "gravity")
        {
            float x = scenedata()->linearforcefield.last()->force.x();
            float y = scenedata()->linearforcefield.last()->force.y();
            OGPhysicsEngine::GetInstance()->SetGravity(x, y);
        }
    }
}


void OGWorld::_ClearPhysics()
{
    pPhysicsEngine_ = 0;

    logDebug("Clear strands");

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

    logDebug("Clear balls");

    while (!balls_.empty()) { delete balls_.takeFirst(); }

    logDebug("Clear static bodies");

    while (!staticBodies_.isEmpty()) { delete staticBodies_.takeFirst(); }

    strandId_ = 0;
    ballId_ = 0;

    OGPhysicsEngine::GetInstance()->Reload();

    isLevelLoaded_ = false;
}

void OGWorld::_ClearScene()
{
    if (!sprites_.isEmpty())
    {
        logDebug("Clear sprites");

        while (!sprites_.isEmpty()) { delete sprites_.takeFirst(); }
    }

    if (!buttons_.isEmpty())
    {
        logDebug("Clear buttons");

        while (!buttons_.isEmpty()) { delete buttons_.takeFirst(); }

    }

    if (pCamera_)
    {
        logDebug("Clear camera");

        delete pCamera_;
        pCamera_ = 0;
    }

    if (pPipe_)
    {
        logDebug("Clear pipe");

        delete pPipe_;
        pPipe_ = 0;
    }
}

void OGWorld::CreateStrand(OGBall* b1, OGBall* b2)
{
    if (b1->id() == -1) { b1->SetId(ballId_++); }
    if (b2->id() == -1) { b2->SetId(ballId_++); }

    OGStrand* obj = new OGStrand(b1, b2, strandId_);
    strands_.insert(strandId_, obj);
    strandId_++;
}

void OGWorld::findNearestAttachedBall()
{
    if (leveldata()->levelexit == 0) { return; }

    qreal x, y, dx, dy, x2, y2, length, tmpLength;
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
                x = x2;
                y = y2;
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
                    x = x2;
                    y = y2;
                    pNearestBall_ = ball;
                }
            }
        }
    }
}

void OGWorld::Update()
{
    if (pPhysicsEngine_) { pPhysicsEngine_->Simulate(); }
}
void OGWorld::CloseLevel()
{
    qDebug("CloseLevel");

    _ClearPhysics();
    _ClearScene();
    _ClearLocalData();
}

void OGWorld::_CreateZOrder()
{
    // Bubble sort
    // Source http://en.wikibooks.org/wiki/Algorithm_Implementation/Sorting/Bubble_sort#C.2B.2B
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
}

void OGWorld::_InsertSprite(OGSprite* sprite)
{
    sprites_ << sprite;
}
