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

    QCache<QString, WOGBall> ballConfigurations_;

    OGBall* nearestBall_;
    float xExit_;
    float yExit_;
    QTimer* timer_;

    QString levelName_;
    QString language_;

    QSize cameraSize_;
    OGCamera currentCamera_;
    int numberCurrentCamera_;

    QSize sceneSize_;
    QString screenType_; // normal or widescren
    QPoint bottomLeft_;
    QRectF window_;

    QList<OGButton*> buttons_;
    QList<OGCamera*> cameras_;
    QList<OGSprite*> sprites_;

    bool isPhysicsEngine_;
    OGPhysicsEngine* physicsEngine_;

    QList<OGBall*> balls_;
    QHash<int, OGStrand*> strands_;
    QList<OGIBody*> staticBodies_;        

    int strandId_;
    int ballId_;

    QSize NormalCamera_() const { return QSize(800, 600); }
    QSize WideScreenCamera_() const { return QSize(1060, 600); }

#define NORMAL_CAMERA NormalCamera_();
#define WIDESCREEN_CAMERA WideScreenCamera_();        

    bool _LoadFX(const QString & path);
    bool _LoadLevel(const QString path);
    bool _LoadMaterials(const QString & path);
    bool _LoadResources(const QString & path, bool share);
    bool _LoadScene(const QString & path);
    bool _LoadText(const QString & path, bool share);

    QPixmap _CreatePixmap(OGSprite* sprite, const QString & image);
    OGSprite* _CreateSprite(const WOGVObject* vobject, const QString & image);

    // level
    OGBall* _CreateBall(WOGBallInstance* ball);
    OGCamera* _CreateCamera(WOGPoi* poi);
    void _CreateStrand(WOGStrand* strand);

    // scene
    void _CreateRadialForcefield();
    void _CreateParticle();
    void _CreateSceneLayer(const WOGSceneLayer & scenelayer
                              , QList<OGSprite*>* sprites
                           );

    void _CreateButtongroup();
    void _CreateButton(const WOGButton & button, QList<OGSprite*>* sprites
                      , QList<OGButton*>* buttons
                       );

    template<class Body, class Data> Body* _CreateBody(Data* data);

    void _CreateLabel();

    void CreatePhysicsScene();
    bool _InitializePhysics();
    void _SetGravity();
    void _ClearPhysics();

    void _ClearScene();

public:
    OGWorld(const QString & levelname=QString(), bool widescreen=false
            , QObject* parent=0);
    virtual ~OGWorld();

    static bool isExist(const QString& path_level);

    // Get properties
    QList<OGBall*> balls() const { return balls_; }
    QList<OGButton*>* buttons() { return &buttons_; }
    QList<OGSprite*> sprites() const { return sprites_; }
    QHash<int, OGStrand*> strands() const { return strands_; }
    QList<OGIBody*>* staticbodies() { return &staticBodies_; }
    OGCamera currentcamera() { return currentCamera_; }
    QString language() const { return language_; }
    WOGLevel* leveldata() { return levelData_; }
    WOGMaterialList* materialdata() { return materialData_; }
    QString levelname() const { return levelName_; }
    WOGScene* scenedata() { return sceneData_; }
    QSizeF scenesize() const { return sceneSize_; }
    OGBall* nearestball() { return nearestBall_; }
    WOGText* textdata() { return textData_[0]; }
    const WOGResources* resrcdata() const { return resourcesData_[0]; }
    QRectF window() const { return window_; }


    // Set properties
    void SetLevelname(const QString & levelname) { levelName_ = levelname; }
    void SetLanguage(const QString & language) { language_ = language; }

    WOGBall* GetBallConfiguration(const QString & type);
    OGCamera* GetCamera(int number=0) { return cameras_.at(number); }
    int GetNumberCameras() const { return cameras_.size(); }


    void SetNextCamera();


    void ClearLocalData();
    void CreateScene();

    bool Initialize();
    bool Load();
    void Reload();
    void CloseLevel();

    void Update();

    template<class Target, class Config>
    Target LoadConf(const QString & path);

    void CreateStrand(OGBall* b1, OGBall *b2);
    void RemoveStrand(OGStrand* strand) { delete strands_.take(strand->id()); }

    void StartSearching() { timer_->start(1000); }

    friend void draw(QPainter* p, OGWorld* w);

private slots:
    void findNearestAttachedBall();

};

#endif // OG_WORLD_H
