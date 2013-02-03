#include "og_world.h"
#include "logger.h"

#include <QPainter>
#include <QFile>

OGWorld::OGWorld(const QString & levelname, bool widescreen)    
{
    numberCurrentCamera_ = 0;
    levelName_ = levelname;

    levelData_ = 0;
    sceneData_ = 0;
    resourcesData_[0] = 0;  // global resources
    resourcesData_[1] = 0;  // local resources
    textData_[0] = 0;       // global text
    textData_[1] = 0;       // local text
    materialData_ = 0;
    effectsData_ = 0;

    if (!widescreen)
    {
        screenType_ = "normal";
        cameraSize_ = NORMAL_CAMERA
    }
    else
    {
        screenType_ = "widescreen";
        cameraSize_ = WIDESCREEN_CAMERA
    }
}

OGWorld::~OGWorld()
{
    Clear(); // clear local data

    logDebug("Clear share data");

    if (resourcesData_[0]) { delete resourcesData_[0]; }

    if (textData_[0]) { delete textData_[0]; }

    if (materialData_) { delete materialData_; }

    if (!effectsData_) { delete effectsData_; }
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
    logDebug("Loading share data...");

    QString path;

    path = "./properties/materials.xml";

    LoadMaterials_(path);

    path = "./properties/resources.xml";    

    if (!LoadResources_(path, true)) { return false; }

    path = "./properties/fx.xml";

    LoadFX_(path);

    path = "./properties/text.xml";

    if (!LoadText_(path, true)) { return false; }

    return true;
}

bool OGWorld::Load()
{
    logDebug("Loading ...");

    if(isExist(levelName_))
        {
            QString path;

            path = "./res/levels/" + levelName_ + "/" + levelName_ + ".level.xml";
            if (!LoadLevel_(path)) { return false; }

            path = "./res/levels/" + levelName_ + "/" + levelName_  + ".scene.xml";
            if (!LoadScene_(path)) { return false; }

            path = "./res/levels/" + levelName_ + "/" + levelName_  + ".resrc.xml";
            if (!LoadResources_(path, false)) { return false; }

            path = "./res/levels/" + levelName_ + "/" + levelName_  + ".text.xml";
            LoadText_(path, false);
        }
    else
        {
            logWarn(QString("Wrong level name: \"%1\"").arg(levelName_));
            return false;
        }

    return true;
}

template<class Target, class Config>
Target OGWorld::Load_(const QString & path)
{
    Config config(path);

    if (config.Open())
    {
        if (config.Read()) { return config.Parser(); }
        else { logWarn("File " + path + " is corrupted"); }
    }
    else { logWarn("File " + path +" not found"); }

    return 0;
}

bool OGWorld::LoadFX_(const QString & path)
{
    logDebug("effects file");

    if (effectsData_) { return false; }

    effectsData_ = Load_<WOGEffects*, OGEffectConfig> (path);

    if (effectsData_) { return true; }
    else { return false; }
}

bool OGWorld::LoadLevel_(const QString path)
{           
    logDebug("level file");

    if (levelData_) { return false; }

    levelData_ = Load_<WOGLevel*, OGLevelConfig> (path);

    if (levelData_) { return true; }
    else { return false; }
}

bool OGWorld::LoadMaterials_(const QString & path)
{
    logDebug("material file");

    if (materialData_) { return false; }

    materialData_ = Load_<WOGMaterialList*, OGMaterialConfig> (path);

    if (materialData_) { return true; }
    else { return false; }
}

bool OGWorld::LoadResources_(const QString & path, bool share)
{
    logDebug("resources file");

    WOGResources* data;

    if (share) data = resourcesData_[0];
    else data = resourcesData_[1];

    if (data) { return false; }

    if (share) { data = Load_<WOGResources*, OGResourceConfig> (path); }
    else { data = Load_<WOGResources*, OGResourceConfig> (path); }

    if (!data) {  return false; }
    else if (share) { resourcesData_[0] = data; }
    else { resourcesData_[1] = data; }

    return true;
}

bool OGWorld::LoadScene_(const QString & path)
{
    logDebug("scene file");

    if (sceneData_) { return false; }

    sceneData_ = Load_<WOGScene*, OGSceneConfig> (path);

    if (sceneData_) { return true; }
    else { return false; }
}

bool OGWorld::LoadText_(const QString & path, bool share)
{
    logDebug("text file");

    WOGText* data;

    if (share) data = textData_[0];
    else data = textData_[1];

    if (data) { return false; }

    OGTextConfig config(path);

    bool status = true;

    if (config.Open())
    {
        if (config.Read())
        {
            data = config.Parser(language_);

            if (!data) { status = false; }
            else if (share) { textData_[0] = data; }
            else { textData_[1] = data; }
        }
        else
        {
            logWarn("File " + path + " is corrupted");

            status = false;
        }
    }
    else
    {
        logWarn("File " + path +" not found");

        status = false;
    }

    return status;
}

void OGWorld::Clear()
{
    logDebug("Clear buttons");

    while (!buttons_.isEmpty()) { delete buttons_.takeFirst(); }

    logDebug("Clear sprites");

    while (!sprites_.isEmpty()) { delete sprites_.takeFirst(); }

    logDebug("Clear cameras");

    while (!cameras_.isEmpty()) { delete cameras_.takeFirst(); }

    logDebug("Clear level");

    if (levelData_) { delete levelData_; }

    levelData_ = 0;

    logDebug("Clear scene");

    if (sceneData_) { delete sceneData_; }

    sceneData_ = 0;

    logDebug("Clear resources");

    if (resourcesData_[1]) { delete resourcesData_[1]; }

    resourcesData_[1] = 0;

    logDebug("Clear text");

    if (textData_[1]) { delete textData_[1]; }

    textData_[1] = 0;
}

void OGWorld::CreateScene()
{
    logDebug("Create scene");

    logDebug("Create scenelayers");

    for (int i=0; i< scenedata()->sceneLayer.size(); i++)
    {
        CreateSceneLayer_(*scenedata()->sceneLayer.at(i), &sprites_);
    }

    logDebug("Create buttongroups");

    for (int i=0; i< scenedata()->buttongroup.size(); i++)
    {
        for (int j=0; j < scenedata()->buttongroup.at(i)->button.size(); j++)
        {
            CreateButton_(*scenedata()->buttongroup.at(i)->button.at(j)
                          , &sprites_ , &buttons_
                          );
        }
    }

    logDebug("Create buttons");

    for (int i=0; i< scenedata()->button.size(); i++)
    {
        CreateButton_(*scenedata()->button.at(i), &sprites_, &buttons_);
    }

    // Create Z-order
    logDebug("Create Z-order");

    // Bubble sort
    // Source http://en.wikibooks.org/wiki/Algorithm_Implementation/Sorting/Bubble_sort#C.2B.2B
    QList <OGSprite*>::iterator first = sprites_.begin();
    QList <OGSprite*>::iterator last = sprites_.end();
    QList <OGSprite*>::iterator i;

    while(first < --last)
    {
        for(i=first; i < last; ++i)
        {
           if ( (*(i+1))->depth < (*i)->depth)
           {
               std::iter_swap( i, i+1 );
           }
        }
    }

    logDebug("Creating cameras");

    for (int i=0; i < leveldata()->camera.size(); i++)
    {
        if (leveldata()->camera.at(i)->aspect == screenType_)
        {
            for (int j=0; j < leveldata()->camera.at(i)->poi.size(); j++)
            {
                cameras_ << CreateCamera_(leveldata()->camera.at(i)->poi.at(j)
                                          );
            }
        }
    }

    currentCamera_ = GetCamera();

    qreal sceneWidth, sceneHeight;

    sceneWidth = qAbs(scenedata()->maxx) + qAbs(scenedata()->minx);
    sceneHeight = qAbs(scenedata()->maxy) +qAbs(scenedata()->miny);

    sceneSize_ = QSizeF(sceneWidth, sceneHeight);
}

void OGWorld::CreateSceneLayer_(const WOGSceneLayer & scenelayer
                                   , QList<OGSprite*>* sprites
                                )
{
    OGSprite* sprite = CreateSprite_(&scenelayer
                                    , scenelayer.image
                                     );

    sprite->visible = true;
    sprites->push_back(sprite);
}

OGCamera* OGWorld::CreateCamera_(WOGPoi* poi)
{
    return new OGCamera(QPointF(poi->position.x(), poi->position.y())
                        , cameraSize_ , poi->zoom, poi->traveltime, poi->pause
                          );
}

void OGWorld::CreateButton_(const WOGButton & button, QList<OGSprite*>* sprites
                           , QList<OGButton*> *buttons
                            )
{
    OGButton* btn;

    btn = new OGButton;

    sprites->push_back(CreateSprite_(&button, button.up));
    btn->position(button.position);
    btn->onclick(button.onclick);
    btn->up(sprites->last());
    btn->size(btn->up()->size);
    btn->up()->visible = true;

    sprites->push_back(CreateSprite_(&button, button.over));
    btn->over(sprites->last());
    btn->over()->visible = false;

    if (!button.disabled.isEmpty())
    {
        sprites->push_back(CreateSprite_(&button
                                        , button.disabled)
                           );

        btn->disabled(sprites->last());
        btn->disabled()->visible = false;
    }

    buttons->push_back(btn);
}

QPixmap OGWorld::CreatePixmap_(OGSprite* sprite, const QString & image)
{
    QImage source(resourcesData_[1]->GetResource(WOGResource::IMAGE
                                             , image) +".png"
                  );

    QTransform transform;
    transform.rotate((-1.0)*sprite->rotation);

    QImage tmpImage(source.transformed(transform, Qt::SmoothTransformation));

    QImage target(tmpImage.size(), QImage::Format_ARGB32_Premultiplied);

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

OGSprite* OGWorld::CreateSprite_(const WOGVObject* vobject
                                 , const QString & image
                                 )
{
    qreal width, height, x, y;
    OGSprite* sprite;

    sprite = new OGSprite;
    sprite->scale = vobject->scale;
    sprite->rotation = vobject->rotation;
    sprite->colorize = vobject->colorize;
    sprite->alpha = vobject->alpha;
    sprite->depth = vobject->depth;
    sprite->image = CreatePixmap_(sprite, image);

    // Set sprite size
    width = sprite->image.width() * sprite->scale.x();
    height = sprite->image.height() * sprite->scale.y();
    sprite->size= QSize(width, height);

    // Set sprite position    
    x = vobject->position.x() - sprite->size.width() * 0.5;
    y = (vobject->position.y() + sprite->size.height() * 0.5)*(-1.0);
    sprite->position = QPointF(x, y);

    return sprite;
}

void OGWorld::SetNextCamera()
{
    if (++numberCurrentCamera_ >= cameras_.size())
    {
        numberCurrentCamera_ = 0;
    }

    currentCamera_ = GetCamera(numberCurrentCamera_);
}
