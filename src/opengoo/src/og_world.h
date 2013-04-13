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

class OGIPipe;

class OGWorld : public QObject
{
        Q_OBJECT

        WOGLevel* pLevelData_;
        WOGScene* pSceneData_;
        WOGResources* pResourcesData_[2];
        WOGText* pTextData_[2];
        WOGMaterialList* pMaterialData_;
        WOGEffects* pEffectsData_;

        QCache<QString, WOGBall> ballConfigurations_;

        OGBall* pNearestBall_;
        float xExit_;
        float yExit_;
        QTimer* pTimer_;

        QString levelName_;
        QString language_;
        bool isLevelLoaded_;

        QList<OGButton*> buttons_;
        QList<OGSprite*> sprites_;

        QList<OGBall*> balls_;
        QHash<int, OGStrand*> strands_;
        QList<OGIBody*> staticBodies_;

        int strandId_;
        int ballId_;

        bool _LoadFX(const QString &path);
        bool _LoadLevel(const QString path);
        bool _LoadMaterials(const QString &path);
        bool _LoadResources(const QString &path, bool share);
        bool _LoadScene(const QString &path);
        bool _LoadText(const QString &path, bool share);

        QPixmap _CreatePixmap(OGSprite* sprite, const QString &image);
        OGSprite* _CreateSprite(const WOGVObject* vobject, const QString &image);

        // level
        OGBall* _CreateBall(WOGBallInstance* ball);
        void _CreateStrand(WOGStrand* strand);

        // scene
        void _CreateRadialForcefield();
        void _CreateParticle();
        void _CreateSceneLayer(const WOGSceneLayer &scenelayer
                               , QList<OGSprite*>* sprites);

        void _CreateButtongroup();
        void _CreateButton(const WOGButton &button, QList<OGSprite*>* sprites
                           , QList<OGButton*>* buttons);

        // Pipe
        OGIPipe* pPipe_;
        WOGPipe* _GetPipeData() { return pLevelData_->pipe; }
        void _CreatePipe();

        template<class Body, class Data> Body* _CreateBody(Data* data);

        void _CreateLabel();

        class OGWindowCamera* pCamera_;
        bool _CreateCamera();

        bool isPhysicsEngine_;
        OGPhysicsEngine* pPhysicsEngine_;

        void CreatePhysicsScene();
        bool _InitializePhysics();
        void _SetGravity();
        void _ClearPhysics();

        void _ClearScene();
        void _ClearLocalData();

        void _CreateZOrder();

    public:
        OGWorld(const QString &levelname = QString(), QObject* parent = 0);
        virtual ~OGWorld();

        static bool isExist(const QString &path_level);

        // Get properties
        const QList<OGBall*>& balls() const { return balls_; }
        const QList<OGButton*>& buttons() const { return buttons_; }
        const QList<OGSprite*>& sprites() const { return sprites_; }
        const QHash<int, OGStrand*>& strands() const { return strands_; }
        QList<OGIBody*>& staticbodies() { return staticBodies_; }
        const QString &language() const { return language_; }
        WOGLevel* leveldata() const { return pLevelData_; }
        WOGMaterialList* materialdata() { return pMaterialData_; }
        const QString &levelname() const { return levelName_; }
        WOGScene* scenedata() const { return pSceneData_; }
        OGBall* nearestball() { return pNearestBall_; }
        WOGText* textdata() { return pTextData_[0]; }
        WOGResources* resrcdata() const { return pResourcesData_[0]; }
        OGIPipe* pipe() const { return pPipe_; }

        bool isLevelLoaded() const { return isLevelLoaded_; }


        // Set properties
        void SetLevelname(const QString &levelname) { levelName_ = levelname; }
        void SetLanguage(const QString &language) { language_ = language; }

        WOGBall* GetBallConfiguration(const QString &type);

        void CreateScene();

        bool Initialize();
        bool Load();
        void Reload();
        void CloseLevel();

        void Update();

        template<class Target, class Config>
        Target LoadConf(const QString &path);

        void CreateStrand(OGBall* b1, OGBall* b2);
        void RemoveStrand(OGStrand* strand) { delete strands_.take(strand->id()); }

        void StartSearching() { pTimer_->start(1000); }

        friend void draw(QPainter* p, OGWorld* w);
        friend class OGPipe;

    private slots:
        void findNearestAttachedBall();

};

#endif // OG_WORLD_H
