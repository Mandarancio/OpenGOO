#ifndef OG_WORLD_H
#define OG_WORLD_H

#include <OGConfiguration>
#include "og_camera.h"
#include "og_button.h"
#include "og_sprite.h"

class OGWorld
{               
    WOGLevel* levelData_;
    WOGScene* sceneData_;        
    WOGResources* resourcesData_[2];
    WOGText* textData_[2];
    WOGMaterialList* materialData_;
    WOGEffects* effectsData_;

    QString levelName_;
    QString language_;

    QSize cameraSize_;
    OGCamera currentCamera_;
    int numberCurrentCamera_;

    QSizeF sceneSize_;
    QString screenType_; // normal or widescren

    QList<OGButton*> buttons_;
    QList<OGCamera*> cameras_;
    QList<OGSprite*> sprites_;

    QSize NormalCamera_() const { return QSize(800, 600); }
    QSize WideScreenCamera_() const { return QSize(1060, 600); }

#define NORMAL_CAMERA NormalCamera_();
#define WIDESCREEN_CAMERA WideScreenCamera_();

    template<class Target, class Config>
    Target Load_(const QString & path);
    bool LoadFX_(const QString & path);
    bool LoadLevel_(const QString path);
    bool LoadMaterials_(const QString & path);
    bool LoadResources_(const QString & path, bool share);
    bool LoadScene_(const QString & path);
    bool LoadText_(const QString & path, bool share);

    QPixmap CreatePixmap_(OGSprite* sprite, const QString & image);
    OGSprite* CreateSprite_(const WOGVObject *vobject, const QString & image);

    // level
    OGCamera* CreateCamera_(WOGPoi* poi);
    void CreateStrand_();

    // scene
    void CreateRadialForcefield_();
    void CreateParticle_();
    void CreateSceneLayer_(const WOGSceneLayer & scenelayer
                              , QList<OGSprite*>* sprites
                           );

    void CreateButtongroup_();
    void CreateButton_(const WOGButton & button, QList<OGSprite*>* sprites
                      , QList<OGButton*>* buttons
                       );

    void CreateLabel_();

public:
    OGWorld(const QString & levelname=QString(), bool widescreen=false);
    virtual ~OGWorld();

    // Get properties
    QList<OGButton*>* buttons() { return &buttons_; }
    QList<OGSprite*>* sprites() { return &sprites_; }
    OGCamera currentcamera() const { return currentCamera_; }
    WOGLevel* leveldata() { return levelData_; }
    QString levelname() const { return levelName_; }
    WOGScene* scenedata() { return sceneData_; }
    QSizeF scenesize() const { return sceneSize_; }


    // Set properties
    void levelname(const QString & levelname) { levelName_ = levelname; }
    void language(const QString & language) { language_ = language; }


    OGCamera GetCamera(int number=0) const { return *cameras_.at(number); }
    int GetNumberCameras() const { return cameras_.size(); }


    void SetNextCamera();


    void Clear();
    void CreateScene();
    bool Initialize();
    bool Load();
};

#endif // OG_WORLD_H
