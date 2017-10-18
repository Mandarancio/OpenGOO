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
#include "GameEngine/scene.h"

#include "SoundEngine/music.h"

#include "entityfactory.h"


typedef std::unique_ptr<ForceField> ForceFieldPtr;
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

class EntityFactory;

typedef QList<OGSprite*> OGSpriteList;

class OGWorld : public QObject, public og::Scene
{
        Q_OBJECT

        EntityFactory& m_entityFactory;
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

        // scene
        void _CreateParticle();
        void _CreateSceneLayer(const WOGSceneLayer &scenelayer
                               , OGSpriteList* sprites);

        void _CreateButtongroup();
        void _CreateButton(const WOGButton &button, OGSpriteList* sprites
                           , QList<OGButton*>* buttons);

        void _CreateCompositeGeom(WOGCompositeGeom* cg);

        // Pipe
        WOGPipe* GetPipeData();

        void _CreateLabel();

        class OGWindowCamera* pCamera_;
        bool _CreateCamera();

        bool isPhysicsEngine_;

        std::unique_ptr<og::physics::PhysicsEngine> m_physicsEngine;

        void CreatePhysicsScene();
        bool _InitializePhysics();
        void _SetGravity();
        void _ClearPhysics();

        void _ClearScene();
        void _ClearLocalData();

        void _CreateZOrder();

        OpenGOO* _GetGame();

        og::physics::PhysicsEngine& GetPhysicsEngine()
        {
            return *m_physicsEngine;
        }

        OGIBody* AddStaticBody(OGIBody* a_body);

        template<class Body>
        Body* _CreateBody(const typename Body::Data& data)
        {
            auto material = materialdata()->GetMaterial(data.material);
            if (!material)
                std::runtime_error(qPrintable("Wrong material id:" + data.material));

            return new Body(GetPhysicsEngine(), data, *material);
        }

    public:
        OGWorld(
            EntityFactory& a_factory,
            const QString& a_levelname,
            const QString& a_language,
            QObject* a_parent = nullptr);

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

        const QString& GetName() const { return levelName_; }

        WOGScene* scenedata() const { return pSceneData_; }

        OGBall* nearestball() { return pNearestBall_; }

        WOGText* textdata() { return pTextData_[0]; }

        WOGResources* resrcdata() const { return pResourcesData_[0]; }

        const std::vector<ForceFieldPtr> &forcefilds() const { return _forceFilds; }

        ForceField& GetForceField(int i) { return *_forceFilds[i]; }

        bool isLevelLoaded() const { return isLevelLoaded_; }

        // Set properties
//        void SetLevelname(const QString &levelname) { levelName_ = levelname; }

//        void SetLanguage(const QString &language) { language_ = language; }

        void CreateScene();

        bool Initialize();
        bool Load();
        bool LoadLevel();
        void Reload();
        void CloseLevel();

        void Update();

        virtual void Render(QPainter& a_p);

        template<class Target, class Config>
        Target LoadConf(const QString &path);

        void CreateStrand(OGBall* b1, OGBall* b2);
        void RemoveStrand(OGStrand* strand);

        EntityPtr AddEntity(EntityPtr a_e);

        void _InsertSprite(OGSprite* sprite);

    private:
        void OnMouseDown(const QPoint& a_point);

        void OnMouseUp(const QPoint& a_point);

        void OnMouseMove(const QPoint& a_point);

        int GetCount() const
        {
            return m_update.size();
        }

        MusicSPtr GetMusic(const QString& aId);

    private slots:
        void findNearestAttachedBall();
};
