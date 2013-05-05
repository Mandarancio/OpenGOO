#include "progresswindow.h"
#include "og_ui.h"
#include "continuebutton.h"
#include "og_uilabel.h"
#include "GameEngine/og_gameengine.h"

struct ProgressWindowImpl
{
    ContinueButton btnContinue;
    OGUILabel labelBalls;
    OGUI ui;
};

ProgressWindow::ProgressWindow(int width, int height)
{
    pImpl_ = new ProgressWindowImpl;

    SetSize(width, height);
    SetBG(QColor(0, 0, 0, 32));

    ContinueButton* btnContinue = &pImpl_->btnContinue;

    btnContinue->SetParent(this);
    btnContinue->SetSize(120, 40);
    btnContinue->SetBG(Qt::black);
    btnContinue->SetFontColor(Qt::white);

    int x = (width / 2) - (btnContinue->width() / 2);
    int y = height - (btnContinue->height() + 20);
    btnContinue->SetPosition(QPoint(x, y));
    btnContinue->SetText("Continue");

    OGUILabel* labelBalls = &pImpl_->labelBalls;

    labelBalls->SetParent(this);
    labelBalls->SetSize(120, 40);
    labelBalls->SetPosition(0, 0);
    labelBalls->SetBG(QColor(Qt::transparent));
    labelBalls->SetFontColor(Qt::white);

    OGUI* ui = &pImpl_->ui;

    ui->Add(btnContinue);
    ui->Add(labelBalls);

    AddUI(ui);

    OGGameEngine::getEngine()->addWindow("WND_PROGRESS", this);
}

ProgressWindow::~ProgressWindow()
{
    OGGameEngine::getEngine()->RemoveWindow("WND_PROGRESS");
    delete pImpl_;
}

void ProgressWindow::SetBalls(int balls)
{
    pImpl_->labelBalls.SetText("Balls: " + QString::number(balls));
}
