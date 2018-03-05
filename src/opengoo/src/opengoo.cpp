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

    m_entityFactory.reset(new EntityFactory());

    if (m_language.isEmpty())
    {
        m_language = "en";
    }

    m_scene = CreateScene("", false);

    if (m_gotoScene.isEmpty())
    {
        m_gotoScene = GetMainMenuName();
    }

    if (flag & FPS)
    {
        _pFPS.reset(new OGFPSCounter(QRect(20, 20, 40, 40)));
    }

    width_ = GE->getWidth();
    height_ = GE->getHeight();

    auto rm = GE->GetResourceManager();
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
    mCamera.SetPosition(0.0f, 0.0f);
    mCamera.SetZoom(1.0f);
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

void OpenGOO::LoadScene(const QString& a_name)
{
    auto scene = CreateScene(a_name);
    m_previousScene = GetScene()->GetName();
    SetScene(scene);
    SetPause(false);
}

std::shared_ptr<Scene> OpenGOO::CreateScene(const QString& a_name, bool aShouldCreatePhysics)
{

#if 0
    PhysicsEngineFactory pef;
    return std::make_shared<OGWorld>(pef, *m_entityFactory, m_gotoScene, m_language);
#else
    return std::make_shared<Scene>(a_name, aShouldCreatePhysics ? og::physics::PhysicsEngine::Create() : nullptr);
#endif
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

    mCamera.Update();
    const auto& pos = mCamera.GetPosition();
    mCamera.SetPosition(pos.x() + mCameraSpeed.x(), pos.y() + mCameraSpeed.y());

    GetScene()->Update();

    if (!m_gotoScene.isEmpty())
    {
        logInfo("Loading Scene..." + m_gotoScene);
        LoadScene(m_gotoScene);
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
        if (GetScene()->GetName().startsWith(QLatin1String("island")))
        {
            GotoScene(GetMainMenuName());
        }
        else if (GetScene()->GetName() != GetMainMenuName())
        {
            GotoScene(m_previousScene);
        }
        return;
    }
}

void OpenGOO::_Quit()
{
    GE->quit();
}

// Main menu
inline QLatin1String OpenGOO::GetMainMenuName()
{
    return QLatin1String("MapWorldView");
}
