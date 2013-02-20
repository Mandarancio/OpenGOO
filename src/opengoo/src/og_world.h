#ifndef OG_WORLD_H
#define OG_WORLD_H

#include <OGConfiguration>
#include "og_camera.h"
#include "og_button.h"
#include "og_sprite.h"
#include "og_ibody.h"
#include "og_ball.h"
#include "og_strand.h"
#include "og_circle.h"
#include "og_rectangle.h"
#include "og_line.h"

#include "physics.h"

#include <QObject>
#include <QCache>
#include <QHash>
#include <QTimer>

struct OGStaticBody
{
    OGPhysicsBody* body;
    QString tag;

    ~OGStaticBody() { delete body; }
};

class OGWorld : public QObject
{
    Q_OBJECT

    WOGLevel* levelData_;
    WOGScene* sceneData_;        
    WOGResources* resourcesData_[2];
    WOGText* textData_[2];
    WOGMaterialList* materialData_;
    WOGEffects* effectsData_;

    OGBall* nearestBall_;
    float xExit_;
    float yExit_;
    QTimer* timer_;

    bool isPhysicsEngine_;

    QString levelName_;
    QString language_;

    QSize cameraSize_;
    OGCamera currentCamera_;
    int numberCurrentCamera_;

    QSizeF sceneSize_;
    QString screenType_; // normal or widescren

    QList<OGBall*> balls_;
    QList<OGButton*> buttons_;
    QList<OGCamera*> cameras_;
    QList<OGSprite*> sprites_;    
    QHash<int, OGStrand*> strands_;
    QList<OGIBody*> staticBodies_;
    QCache<QString, WOGBall> ballConfigurations_;

    int strandId_;
    int ballId_;

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
    OGSprite* CreateSprite_(const WOGVObject* vobject, const QString & image);

    // level
    OGBall* CreateBall_(WOGBallInstance* ball);
    OGCamera* CreateCamera_(WOGPoi* poi);
    void CreateStrand_(WOGStrand* strand);

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

    template<class Body, class Data> Body* CreateBody_(Data* data);

    void CreateLabel_();

    bool InitializePhysics_();    

public:
    OGWorld(const QString & levelname=QString(), bool widescreen=false
            , QObject* parent=0);
    virtual ~OGWorld();

    static bool isExist(const QString& path_level);

    // Get properties
    QList<OGBall*>* balls() { return &balls_; }
    QList<OGButton*>* buttons() { return &buttons_; }
    QList<OGSprite*>* sprites() { return &sprites_; }
    QHash<int, OGStrand*>* strands() { return &strands_; }
    QList<OGIBody*>* staticbodies() { return &staticBodies_; }
    OGCamera currentcamera() const { return currentCamera_; }
    WOGLevel* leveldata() { return levelData_; }
    WOGMaterialList* materialdata() { return materialData_; }
    QString levelname() const { return levelName_; }
    WOGScene* scenedata() { return sceneData_; }
    QSizeF scenesize() const { return sceneSize_; }
    OGBall* nearestball() { return nearestBall_; }


    // Set properties
    void levelname(const QString & levelname) { levelName_ = levelname; }
    void language(const QString & language) { language_ = language; }

    WOGBall* GetBallConfiguration(const QString & type);
    OGCamera* GetCamera(int number=0) { return cameras_.at(number); }
    int GetNumberCameras() const { return cameras_.size(); }


    void SetNextCamera();


    void Clear();
    void CreateScene();
    bool Initialize();
    bool Load();

    void CreateStrand(OGBall* b1, OGBall *b2);
    void RemoveStrand(OGStrand* strand) { delete strands_.take(strand->id()); }

    void StartSearching() { timer_->start(1000); }

private slots:
    void findNearestAttachedBall();

};

#endif // OG_WORLD_H
