#include "progresswindow.h"
#include "og_utils.h"


using namespace og::ui;
using namespace ogUtils;

//struct ProgressWindow::Impl
//{
//    Frame frame;
//    ContinueButton btnContinue;
//    Label labelBalls;
//};

ProgressWindow::ProgressWindow() : _pImpl(new Impl)
{    
    auto ge = getGameEngine();

    auto frame = &_pImpl->frame;
    {
        int w = 400;
        int h = 400;
        int x = ge->getWidth() / 2 - w / 2;
        int y = ge->getHeight() / 2 - h / 2;
        frame->setSize(w, h);
        frame->setPosition(x, y);
        frame->setBgColor(QColor(0, 0, 0, 25));
        frame->setVisible(true);
    }

    auto btn = &_pImpl->btnContinue;
    {
        int x = frame->x() + frame->width() / 2 - btn->width() / 2;
        int y = frame->y() + frame->height() - (btn->height() + 50);
        btn->setPosition(x, y);
        connect(btn, SIGNAL(pressed()), this, SIGNAL(close()));
        btn->setVisible(true);
    }

    auto label = &_pImpl->labelBalls;
    {
        int w = 120;
        int h = 80;
        int x = frame->x() + 50;
        int y = frame->y() + 50;
        label->setSize(w, h);
        label->setPosition(x, y);
        label->setVisible(true);
    }
}

void ProgressWindow::setBalls(int balls, int extraBalls)
{
    _pImpl->labelBalls.setText("Balls: " + QString::number(balls) + "\n\n"
                                "Extra balls: " + QString::number(extraBalls));
}
