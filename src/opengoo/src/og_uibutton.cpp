#include "og_uibutton.h"
#include "og_world.h"
#include "og_gameengine.h"
#include "og_event.h"

extern OGWorld* _world;

OGUIButton::OGUIButton(WOGButton* config)
    : upImg_(0), overImg_(0), onclick_(config->onclick)
{     
    text_ = _world->textdata()->GetString(config->text);
    config_ = config;    
}

OGUIButton::~OGUIButton()
{
    if (upImg_ != 0) { delete upImg_; }
    if (overImg_ != 0) { delete overImg_; }
}

void OGUIButton::SetUpImage(const QString &path)
{
    upImg_ = new QImage(path);
    img_ = upImg_;

    int w = qRound(upImg_->width() * config()->scale.x());
    int h = qRound(upImg_->height() * config()->scale.y());

    setWidth(w);
    setHeight(h);

    w = OGGameEngine::getEngine()->getWidth();
    h = OGGameEngine::getEngine()->getHeight();

    int x = qRound(w * 0.5f + config()->position.x());
    int y = qRound(h * 0.5f - config()->position.y());

    moveCenter(QPoint(x, y));
}

void OGUIButton::Paint(QPainter* p)
{
    float w = width();
    float h = height();
    float x1 = x();
    float y1 = y();

    QRectF target(x1, y1, w, h);

    if (img_ != 0) { p->drawImage(target, *img_, img_->rect()); }

    QPen pen(Qt::white,  2.0f);
    p->setOpacity(1.0f);
    p->save();

    p->setPen(pen);
    p->setFont(QFont("Arial", 14, QFont::Bold));
    p->drawText(target, Qt::AlignCenter, text_);

    p->restore();
}

void OGUIButton::MouseDown(QMouseEvent *e)
{
    Q_UNUSED(e);

    if (onclick().compare("createretrymenu") == 0)
    {
        SendEvent(new OGMenuEvent("RetryMenu"));
    }
    else if (onclick().compare("createmenu") == 0)
    {        
        SendEvent(new OGMenuEvent("GameMenu"));
    }
    else if (onclick().compare("restartlevelrightnow") == 0)
    {
        SendEvent(new OGEvent(OGEvent::RESTART));
    }
    else if (onclick().compare("showocdcriteria") == 0)
    {
        SendEvent(new OGEvent(OGEvent::SHOW_OCD));
    }
    else if (onclick().compare("backtoisland") == 0)
    {
        SendEvent(new OGEvent(OGEvent::BACKTO_ISLAND));
    }
    else if (onclick().compare("resumegame") == 0)
    {
        SendEvent(new OGEvent(OGEvent::RESUME));
    }
    else if (onclick().compare("backtomainmenu") == 0)
    {
        SendEvent(new OGEvent(OGEvent::BACKTO_MAINMENU));
    }
}

void OGUIButton::MouseMove(QMouseEvent *e)
{
    Q_UNUSED(e);

    if (overImg_ != 0) { img_ = overImg_; }
}

void OGUIButton::Leave(QMouseEvent *e)
{
    Q_UNUSED(e);
    img_ = upImg_;
}
