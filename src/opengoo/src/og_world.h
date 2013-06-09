#ifndef OG_WORLD_H
#define OG_WORLD_H

#include "wog_scene.h"
#include "wog_ball.h"
#include <OGPhysicsEngine>
#include "og_forcefield.h"

#include <QCache>
#include <QHash>
#include <QList>
#include <QPainter>

#include <memory>

typedef std::unique_ptr<physics::OGForceField> ptr_ForceField;
typedef std::unique_ptr<physics::OGRadialForceField> ptr_RForceField;
typedef std::unique_ptr<physics::OGLinearForceField> ptr_LForceField;

struct WOGLevel;
struct WOGText;
struct WOGMaterialList;
struct WOGEffects;
struct WOGBallInstance;
struct WOGStrand;
struct WOGPipe;
struct WOGLevelExit;

class WOGResources;

struct OGSprite;

class OGIPipe;
class Exit;

class OGBall;
class OGButton;
class OGStrand;
class OGIBody;
class OpenGOO;

class QTimer;

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

        // Exit
        Exit* pExit_;
        float xExit_;
        float yExit_;

        QTimer* pTimer_;

        QString levelName_;
        QString language_;
        bool isLevelLoaded_;

        QList<OGButton*> buttons_;

        QList<OGSprite*> sprites_;
        void _InsertSprite(OGSprite* sprite);

        QList<OGBall*> balls_;
        QHash<int, OGStrand*> strands_;
        QList<OGIBody*> staticBodies_;
        std::vector<ptr_ForceField> _forceFilds;

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
        ptr_RForceField _CreateRadialForcefield(WOGRadialForceField* ff);
        void _CreateParticle();
        void _CreateSceneLayer(const WOGSceneLayer &scenelayer
                               , QList<OGSprite*>* sprites);

        void _CreateButtongroup();
        void _CreateButton(const WOGButton &button, QList<OGSprite*>* sprites
                           , QList<OGButton*>* buttons);

        void _CreateCompositeGeom(WOGCompositeGeom* cg);

        // Pipe
        OGIPipe* pPipe_;
        WOGPipe* _GetPipeData();
        void _CreatePipe();

        template<class Body, class Data> Body* _CreateBody(Data* data);

        void _CreateLabel();

        class OGWindowCamera* pCamera_;
        bool _CreateCamera();

        bool isPhysicsEngine_;
        og::OGPhysicsEngine* pPhysicsEngine_;

        void CreatePhysicsScene();
        bool _InitializePhysics();
        void _SetGravity();
        void _ClearPhysics();

        void _ClearScene();
        void _ClearLocalData();

        void _CreateZOrder();

        OpenGOO* _GetGame();

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
        Exit* exit() const { return pExit_; }
        const std::vector<ptr_ForceField> &forcefilds() const { return _forceFilds; }

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

        template<class Target, class Config>
        Target LoadConf(const QString &path);

        void CreateStrand(OGBall* b1, OGBall* b2);
        void RemoveStrand(OGStrand* strand);

        void StartSearching();
        friend class OGPipe;        

    private slots:
        void findNearestAttachedBall();
};

#endif // OG_WORLD_H
