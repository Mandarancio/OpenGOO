#include "og_uibutton.h"
#include "og_world.h"
#include "og_gameengine.h"
#include "og_event.h"
#include "opengoo.h"

OGUIButton::OGUIButton(WOGButton* config)
    : pUpImg_(0)
    , pOverImg_(0)
    , onclick_(config->onclick)
{
    text_ = _GetText(config->text);
    pConfig_ = config;
}

OGUIButton::~OGUIButton()
{
    delete pUpImg_;
    delete pOverImg_;
}

QString OGUIButton::_GetText(QString &str)
{
    return OpenGOO::instance()->GetWorld()->textdata()->GetString(str);
}

void OGUIButton::SetUpImage(const QString &path)
{
    pUpImg_ = new QImage(path);
    pImg_ = pUpImg_;

    int w = qRound(pUpImg_->width() * pConfig_->scale.x());
    int h = qRound(pUpImg_->height() * pConfig_->scale.y());

    setWidth(w);
    setHeight(h);

    w = OGGameEngine::getEngine()->getWidth();
    h = OGGameEngine::getEngine()->getHeight();

    int x = qRound(w * 0.5f + pConfig_->position.x());
    int y = qRound(h * 0.5f - pConfig_->position.y());

    moveCenter(QPoint(x, y));
}

void OGUIButton::SetOverImage(const QString &path)
{
    pOverImg_ = new QImage(path);
}

void OGUIButton::Paint(QPainter* painter)
{
    QRectF target(x(), y(), width(), height());

    if (pImg_) { painter->drawImage(target, *pImg_, pImg_->rect()); }

    QPen pen(Qt::white,  2.0f);
    painter->setOpacity(1.0f);
    painter->save();

    painter->setPen(pen);
    painter->setFont(QFont("Arial", 14, QFont::Bold));
    painter->drawText(target, Qt::AlignCenter, text_);

    painter->restore();
}

void OGUIButton::MouseDown(QMouseEvent* ev)
{
    Q_UNUSED(ev);

    if (onclick().compare("createretrymenu") == 0)
    {
        OpenGOO::SendEvent(new OGMenuEvent("RetryMenu"));
    }
    else if (onclick().compare("createmenu") == 0)
    {
        OpenGOO::SendEvent(new OGMenuEvent("GameMenu"));
    }
    else if (onclick().compare("restartlevelrightnow") == 0)
    {
        OpenGOO::SendEvent(new OGEvent(OGEvent::RESTART));
    }
    else if (onclick().compare("showocdcriteria") == 0)
    {
        OpenGOO::SendEvent(new OGEvent(OGEvent::SHOW_OCD));
    }
    else if (onclick().compare("backtoisland") == 0)
    {
        OpenGOO::SendEvent(new OGEvent(OGEvent::BACKTO_ISLAND));
    }
    else if (onclick().compare("resumegame") == 0)
    {
        OpenGOO::SendEvent(new OGEvent(OGEvent::RESUME));
    }
    else if (onclick().compare("backtomainmenu") == 0)
    {
        OpenGOO::SendEvent(new OGEvent(OGEvent::BACKTO_MAINMENU));
    }
}

void OGUIButton::MouseMove(QMouseEvent* ev)
{
    Q_UNUSED(ev);

    if (pOverImg_) { pImg_ = pOverImg_; }
}

void OGUIButton::Leave(QMouseEvent* ev)
{
    Q_UNUSED(ev);

    pImg_ = pUpImg_;
}
