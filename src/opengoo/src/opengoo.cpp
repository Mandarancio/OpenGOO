/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <functional>

#include <QMouseEvent>
#include <QDebug>

#include "opengoo.h"
#include "og_world.h"
#include "flags.h"
#include "GameEngine/og_gameengine.h"
#include "GameEngine/input.h"
#include <logger.h>
#include "og_windowcamera.h"
#include "og_ballconfig.h"
#include "og_sprite.h"
#include "og_ibody.h"
#include "og_strand.h"
#include "og_button.h"
#include "continuebutton.h"
#include "og_fpscounter.h"
#include "og_forcefield.h"
#include "entityfactory.h"
#include "physicsenginefactory.h"
#include "og_utils.h"
#include "sceneloader.h"

using namespace og;

const char* OpenGOO::types[] = {".level", ".scene", ".resrc"};
const char* OpenGOO::exts[] = {".xml", ".bin"};

OpenGOO* OpenGOO::pInstance_ = nullptr;

OpenGOO::OpenGOO()
{
}

OpenGOO::~OpenGOO()
{
}

OpenGOO* OpenGOO::GetInstance()
{
    if (!pInstance_)
    {
        pInstance_ = new OpenGOO;
    }

    return pInstance_;
}

void OpenGOO::Destroy()
{
    delete pInstance_;
    pInstance_ = nullptr;
}

void OpenGOO::AddSprite(float depth, OGSprite* sprite)
{
    if (layers_.contains(depth))
    {
        layers_[depth].Add(sprite);
    }
    else
    {
        auto it = layers_.insert(depth, OGLayer());
        it->Add(sprite);
    }
}

void OpenGOO::AddSprite(OGSprite* sprite)
{
    AddSprite(sprite->GetDepth(), sprite);
}

void OpenGOO::ClearSprites()
{
    _ClearLayers();
}

void OpenGOO::SetLanguage(const QString &language)
{
    m_language = language;
}

void OpenGOO::_Start()
{
    SetPause(false);

    //initialize randseed
#ifdef DEBUG
    qsrand(0);
#else
    qsrand(QTime::currentTime().toString("hhmmsszzz").toUInt());
#endif

    m_entityFactory.reset(new EntityFactory(*GE->getResourceManager()));

    if (m_language.isEmpty())
    {
        m_language = "en";
    }

    SetScene(std::make_shared<og::Scene>(""));

    if (m_gotoScene.isEmpty())
    {
        m_gotoScene = _GetMainMenu();
    }

    if (flag & FPS)
    {
        _pFPS.reset(new OGFPSCounter(QRect(20, 20, 40, 40)));
    }

    width_ = GE->getWidth();
    height_ = GE->getHeight();

    timeScrollStep_ = width_ / 1000.0f;
    timeStep_ =  GE->getFrameDelay();

    auto rm = GE->getResourceManager();
    if (!rm->ParseResourceFile("./properties/resources.xml"))
    {
        logError("Could not load resources");
    }

    if (!rm->ParseTextFile("./properties/text.xml", m_language))
    {
        logError("Could not load texts");
    }

    if (!rm->ParseFxFile("./properties/fx.xml"))
    {
        logError("Could not load fx");
    }

    mCamera.SetSize(width_, height_);
    mCamera.SetPosition(0, 0);
    mCamera.SetZoom(1);
    mCameraSpeed = QPointF();

    m_deltaTime = 0;
    lastTime_ = QDateTime::currentDateTime();
}

void OpenGOO::_End()
{
    _pFPS.reset();

    if (m_scene)
    {        
        logDebug("Clear Scene");
        m_scene.reset();
    }
}

void OpenGOO::_Activate()
{
    SetPause(false);
}

void OpenGOO::_Deactivate()
{
    SetPause(true);
}

bool OpenGOO::LevelIsExists(const QString& a_name)
{
    const auto path = QString("./res/levels/%1/%1").arg(a_name);

    for (size_t i = 0; i << utils::array_size(types); ++i)
    {
        auto pathWithType = path + types[i];
        for (size_t j = 0; j < utils::array_size(exts); ++j)
        {
            const auto fullPath = pathWithType + exts[j];
            if (!QFile::exists(fullPath))
            {
                logError(QString("File %1 not found").arg(fullPath));
                return false;
            }
        }
    }

    return true;
}

void OpenGOO::LoadScene(og::Scene* a_scene)
{
//        if (!world->Initialize())
//            return;

    const auto& name = a_scene->GetName();
    if (name.isEmpty() || !LevelIsExists(name))
    {
        return;
    }

//    if (world->Load())
    {
//            if (world->leveldata()->visualdebug)
//                SetDebug(*world, true);

//        _SetBackgroundColor(world->GetBackgroundColor());

//        pCamera_ = OGWindowCamera::instance();
        SetPause(false);
    }
}

std::shared_ptr<Scene> OpenGOO::CreateScene()
{
//    PhysicsEngineFactory pef;
    return std::make_shared<Scene>(m_gotoScene);
//    return std::make_shared<OGWorld>(pef, *m_entityFactory, m_gotoScene, m_language);
}

void OpenGOO::_Cycle()
{   
    auto now = QDateTime::currentDateTime();
    m_deltaTime = lastTime_.msecsTo(now);

    if (flag & FPS)
    {
        _pFPS->Update(m_deltaTime);
    }

    lastTime_= now;

    if (isPause())
    {
        return;
    }

    Scroll();

    const auto& pos = mCamera.GetPosition();
    mCamera.SetPosition(pos.x() + mCameraSpeed.x(), pos.y() + mCameraSpeed.y());

    GetScene()->Update();

    if (!m_gotoScene.isEmpty())
    {   
        m_previousScene = GetScene()->GetName();

        SetScene(CreateScene());
        SceneLoader sl;
        logInfo("Loading Scene...");
        if (sl.load(*m_scene))
        {
            logInfo("Scene was loaded");
        }
        else
        {
            logInfo("Could not load Scene");
        }

        m_gotoScene.clear();
    }
}

void OpenGOO::_Paint(QPainter* painter)
{
    int w = qRound(mCamera.GetScaledWidth());
    int h = qRound(mCamera.GetScaledHeight());
    int x = qRound(mCamera.GetPosition().x() + -(w * 0.5));
    int y = qRound(-mCamera.GetPosition().y() + -(h * 0.5));
    painter->setWindow(x, y, w, h);

//        // Paint a scene
//        for (auto it = layers_.begin(); it != layers_.end(); ++it)
//        {
//            (*it).Paint(*painter);
//        }

//        foreach(auto body, pWorld_->staticbodies())
//        {
//            body->Draw(painter);
//        }

//        foreach(auto ball, pWorld_->balls())
//        {
//            ball->Paint(painter, pWorld_->leveldata()->visualdebug);
//        }

//        foreach(auto strand, pWorld_->strands())
//        {
//            strand->Paint(painter, pWorld_->leveldata()->visualdebug);
//        }

//        if (pWorld_->leveldata() && pWorld_->leveldata()->visualdebug)
//        {
//            visualDebug(painter, pWorld_.get(), pCamera_->zoom());
//        }

    if (m_scene)
    {
        m_scene->Render(*painter);
    }
}

void OpenGOO::MouseButtonDown(const QPoint& aPos)
{
    if (isPause())
    {
        return;
    }

    m_scene->OnMouseDown(aPos);
}

void OpenGOO::MouseButtonUp(const QPoint& aPos)
{
    if (isPause())
    {
        return;
    }

    m_scene->OnMouseUp(aPos);
}

void OpenGOO::_MouseMove(QMouseEvent* /*ev*/)
{
//    if (isPause() || !pCamera_ || pProgressWnd_)
//        return;

//    curMousePos_ = ev->pos();

//    QPoint pos = pCamera_->windowToLogical(ev->pos());
//    lastMousePos_ = pos;

//    Q_FOREACH(OGButton * button, pWorld_->buttons())
//    {
//        if (button->TestPoint(pos))
//        {
//            button->up()->SetVisible(false);
//            button->over()->SetVisible(true);
//        }
//        else
//        {
//            button->up()->SetVisible(true);
//            button->over()->SetVisible(false);
//        }
//    }

//    if (!_pSelectedBall)
//    {
//        Q_FOREACH(OGBall * ball, pWorld_->balls())
//        {
//            if (ball->TestPoint(pos))
//            {
//                _pSelectedBall = ball;
//                _pSelectedBall->SetMarked(true);
//                break;
//            }
//        }
//    }
//    else if (_pSelectedBall->IsDragging())
//    {
//        _pSelectedBall->MouseMove(pos);
//    }
//    else if (!_pSelectedBall->TestPoint(pos))
//    {
//        _pSelectedBall->SetMarked(false);
//        _ClearSelectedBall();
//    }
}

void OpenGOO::_KeyDown(QKeyEvent* ev)
{
    switch (ev->key())
    {
    case Qt::Key_Escape:
        GotoScene(m_previousScene);
        return;
    }
}

inline void OpenGOO::_ClearLayers()
{
    layers_.clear();
}

void OpenGOO::_Quit()
{
    GE->quit();
}

void OpenGOO::Scroll()
{
    static const float OFFSET = 50.0f;
    static const int DEFAULT_H_SPEED = 4;
    static const int DEFAULT_V_SPEED = 4;

    curMousePos_ = og::MouseInput::GetPosition();
    float sx = curMousePos_.x();
    float sy = curMousePos_.y();

    if (sx <= OFFSET)
    {
        mCameraSpeed.setX(-DEFAULT_H_SPEED);
    }
    else if (sx >= width_ - OFFSET)
    {
        mCameraSpeed.setX(DEFAULT_H_SPEED);
    }
    else
    {
        mCameraSpeed.setX(0);
    }

    if (sy <= OFFSET)
    {
        mCameraSpeed.setY(DEFAULT_V_SPEED);
    }
    else if (sy >= height_ - OFFSET)
    {
        mCameraSpeed.setY(-DEFAULT_V_SPEED);
    }
    else
    {
        mCameraSpeed.setY(0);
    }
}

void OpenGOO::SetDebug(OGWorld& /*a_world*/, bool /*a_debug*/)
{
//    foreach (auto body, a_world.staticbodies())
//    {
//        body->SetDebug(a_debug);
//    }
}

void OpenGOO::_CreateContinueButton()
{
    pContinueBtn_.reset(new ContinueButton);
    auto btn = pContinueBtn_.get();
    int x = width_ - (btn->width() + 20);
    int y = 20;
    btn->setPosition(x, y);
    connect(btn, SIGNAL(pressed()), this, SLOT(_closeContinueButton()));
    btn->setVisible(true);
}

void OpenGOO::_InitProgressWindow()
{
    pProgressWnd_.reset(new ProgressWindow);
    auto wnd = pProgressWnd_.get();
    connect(wnd, SIGNAL(close()), this, SLOT(_closeProgressWindow()));
    pProgressWnd_->setBalls(balls_, balls_ - ballsRequired_);
}

// Level
bool OpenGOO::_LoadLevel(OGWorld* a_world, const QString& /*levelname*/)
{
    assert(false);
    assert(a_world);
//    auto isLoaded = a_world->LoadLevel();
//    if (!isLoaded)
//        return false;
//    if (a_world->leveldata()->visualdebug)
//        SetDebug(*a_world, true);

//    _SetBackgroundColor(pWorld_->scenedata()->backgroundcolor);

//    pCamera_ = OGWindowCamera::instance();
//    SetPause(false);
    return true;
}

void OpenGOO::_CloseLevel()
{
    assert(false);
//    pWorld_->CloseLevel();
//    _ClearLayers();
//    pCamera_ = 0;
//    mSceneIsLoaded = false;
}

void OpenGOO::ReloadLevel()
{
    assert(false);
//    pWorld_->Reload();
//    pCamera_->SetLastPosition();
//    balls_ = 0;
}
void OpenGOO::_CreateLevel(const QString &levelname)
{
    pLevel_.reset(new Level(levelname));
    auto lvl = pLevel_.get();
    connect(lvl, SIGNAL(closeLevel()), this, SLOT(_backToIsland()));
}

void OpenGOO::_RemoveLevel()
{
    pLevel_.reset();
}

// Main menu
inline QString OpenGOO::_GetMainMenu()
{
    return "MapWorldView";
}

inline void OpenGOO::_LoadMainMenu()
{
    _LoadLevel(GetWorld(), _GetMainMenu());
}

inline void OpenGOO::_CloseMainMenu()
{
    _CloseLevel();
}

// Island public interface
void OpenGOO::LoadIsland(const QString &name)
{
    _CloseMainMenu();
    _CreateIsland(name);
}

inline void OpenGOO::_CreateIsland(const QString &name)
{   
    pIsland.reset(new Island(name));
    auto island = pIsland.get();
    connect(island, SIGNAL(close()), this, SLOT(_backToMainMenu()));
}

inline void OpenGOO::_RemoveIsland()
{
    pIsland.reset();
}

// Slots
void OpenGOO::_backToMainMenu()
{
    _RemoveIsland();
    _LoadMainMenu();
}

void OpenGOO::_backToIsland()
{
    _RemoveLevel();
    _CreateIsland(_GetIsland());
}

void OpenGOO::loadLevel(const QString &levelname)
{
     _RemoveIsland();
     _CreateLevel(levelname);
}

void OpenGOO::_closeContinueButton()
{
    pContinueBtn_.reset();
    _InitProgressWindow();
}

void OpenGOO::_closeProgressWindow()
{
    pProgressWnd_.reset();
    _backToIsland();
}
