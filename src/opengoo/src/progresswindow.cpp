#include "progresswindow.h"
#include "og_ui.h"
#include "continuebutton.h"
#include "og_uilabel.h"
#include "GameEngine/og_gameengine.h"

#include <QPainter>

struct ProgressWindowImpl
{
    ContinueButton btnContinue;
    OGUILabel labelBalls;
    OGUI* pUI;
};

ProgressWindow::ProgressWindow(int width, int height)
{
    pImpl_ = new ProgressWindowImpl;

    SetSize(width, height);
    SetBG(QColor(0, 0, 0, 32));

    pImpl_->btnContinue.SetParent(this);
    pImpl_->btnContinue.SetSize(120, 40);

    pImpl_->labelBalls.SetParent(this);
    pImpl_->labelBalls.SetSize(120, 40);
    pImpl_->labelBalls.SetPosition(0, 0);
    pImpl_->labelBalls.SetBG(QColor(Qt::transparent));
    pImpl_->labelBalls.SetFontColor(Qt::white);

    pImpl_->pUI = new OGUI;
    pImpl_->pUI->Add(&pImpl_->btnContinue);
    pImpl_->pUI->Add(&pImpl_->labelBalls);

    AddUI(pImpl_->pUI);

    OGGameEngine::getEngine()->addWindow("WND_PROGRESS", this);
}

ProgressWindow::~ProgressWindow()
{
    OGGameEngine::getEngine()->RemoveWindow("WND_PROGRESS");
    delete pImpl_->pUI;
    delete pImpl_;
}

void ProgressWindow::Init()
{
    QSize wndSize = GetSize();
    QSize btnSize = pImpl_->btnContinue.GetSize();
    int x = (wndSize.width() / 2) - (btnSize.width() / 2);
    int y = wndSize.height() - (btnSize.height() + 20);
    pImpl_->btnContinue.SetPosition(QPoint(x, y));
    pImpl_->btnContinue.SetText("Continue");
    pImpl_->btnContinue.Show();
    pImpl_->labelBalls.Show();
}

void ProgressWindow::SetBalls(int balls)
{
    pImpl_->labelBalls.SetText("Balls: " + QString::number(balls));
}
