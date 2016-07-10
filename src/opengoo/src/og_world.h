#pragma once

#include <memory>

#include <QCache>
#include <QHash>
#include <QList>
#include <QPainter>

#include "wog_scene.h"
#include "wog_ball.h"
#include <OGPhysicsEngine>
#include "og_forcefield.h"
#include "og_sprite.h"
#include "GameEngine/entity.h"
#include "radialforcefield.h"
#include "GameEngine/scene.h"


typedef std::unique_ptr<ForceField> ForceFieldPtr;
typedef std::unique_ptr<RadialForceField> RadialForceFieldPtr;
typedef std::unique_ptr<LinearForceField> LinearForceFieldPtr;

struct WOGLevel;
struct WOGText;
struct WOGMaterialList;
struct WOGEffects;
struct WOGBallInstance;
struct WOGStrand;
struct WOGPipe;
struct WOGLevelExit;

class WOGResources;

class OGSprite;

class Exit;

class OGBall;
class OGButton;
class OGStrand;
class OGIBody;
class OpenGOO;

class QTimer;

class EntityFactory;

typedef QList<OGSprite*> OGSpriteList;

class OGWorld : public QObject, public og::Scene
{
        Q_OBJECT

        EntityFactory& m_factory;
        WOGLevel* pLevelData_;
        WOGScene* pSceneData_;
        WOGResources* pResourcesData_[2];
        WOGText* pTextData_[2];
        WOGMaterialList* pMaterialData_;
        WOGEffects* pEffectsData_;

        QCache<QString, WOGBall> ballConfigurations_;

        OGBall* pNearestBall_;

        // Exit
        float xExit_;
        float yExit_;

        QTimer* pTimer_;

        QString levelName_;
        QString language_;
        bool isLevelLoaded_;

        QList<OGButton*> buttons_;

        OGSpriteList sprites_;
        QList<EntityPtr> m_update;
        QList<EntityPtr> m_added[2];
        int m_num_added;

        QList<OGBall*> balls_;
        QHash<int, OGStrand*> strands_;
        QList<OGIBody*> staticBodies_;
        std::vector<ForceFieldPtr> _forceFilds;

        int strandId_;
        int ballId_;

        WOGCompositeGeom _cg; // compositgeom

        bool _LoadFX(const QString &path);
        bool _LoadLevel(const QString path);
        bool _LoadMaterials(const QString &path);
        bool _LoadResources(const QString &path, bool share);
        bool _LoadScene(const QString &path);
        bool _LoadText(const QString &path, bool share);

        OGSprite* _CreateSprite(const WOGVObject* vobject, const QString &image);

        // level
        OGBall* _CreateBall(WOGBallInstance* ball);
        void _CreateStrand(WOGStrand* strand);

        // scene
        RadialForceFieldPtr _CreateRadialForcefield(WOGRadialForceField* ff);
        void _CreateParticle();
        void _CreateSceneLayer(const WOGSceneLayer &scenelayer
                               , OGSpriteList* sprites);

        void _CreateButtongroup();
        void _CreateButton(const WOGButton &button, OGSpriteList* sprites
                           , QList<OGButton*>* buttons);

        void _CreateCompositeGeom(WOGCompositeGeom* cg);

        // Pipe
        WOGPipe* GetPipeData();

        template<class Body, class Data> Body* _CreateBody(Data* data);

        void _CreateLabel();

        class OGWindowCamera* pCamera_;
        bool _CreateCamera();

        bool isPhysicsEngine_;
        og::physics::PhysicsEngine* pPhysicsEngine_;

        void CreatePhysicsScene();
        bool _InitializePhysics();
        void _SetGravity();
        void _ClearPhysics();

        void _ClearScene();
        void _ClearLocalData();

        void _CreateZOrder();

        OpenGOO* _GetGame();

        og::physics::PhysicsEngine* GetPhisicsEngine();

        OGIBody* AddStaticBody(OGIBody* a_body);

    public:
        OGWorld(EntityFactory& a_factory,
                const QString &levelname = QString(),
                QObject* parent = 0);
        virtual ~OGWorld();

        static bool isExist(const QString &path_level);

        // Get properties
        const QList<OGBall*>& balls() const { return balls_; }
        const QList<OGButton*>& buttons() const { return buttons_; }
        const OGSpriteList& sprites() const { return sprites_; }
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

        const std::vector<ForceFieldPtr> &forcefilds() const { return _forceFilds; }

        ForceField& GetForceField(int i)
        {
            return *_forceFilds[i];
        }

        bool isLevelLoaded() const { return isLevelLoaded_; }


        // Set properties
        void SetLevelname(const QString &levelname) { levelName_ = levelname; }
        void SetLanguage(const QString &language) { language_ = language; }

        WOGBall* GetBallConfiguration(const QString &type);

        void CreateScene();

        bool Initialize();
        bool Load();
        bool LoadLevel(const QString &levelname);
        void Reload();
        void CloseLevel();

        void Update();

        virtual void Render(QPainter& a_p);

        template<class Target, class Config>
        Target LoadConf(const QString &path);

        void CreateStrand(OGBall* b1, OGBall* b2);
        void RemoveStrand(OGStrand* strand);

        ImageSourcePtr CreateImageSource(const QString& a_id);

        EntityPtr AddEntity(EntityPtr a_e);

        void StartSearching();

        void _InsertSprite(OGSprite* sprite);

    private slots:
        void findNearestAttachedBall();
};
