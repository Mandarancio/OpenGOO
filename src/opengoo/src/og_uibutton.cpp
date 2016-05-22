#include "og_uibutton.h"
#include "og_world.h"
#include "og_gameengine.h"
#include "opengoo.h"
#include "wog_text.h"
#include "og_sprite.h"

#include <QImage>
#include <QString>
#include <QTransform>

struct og::ui::OGUIButtonImpl
{
    QString onclick;
    QString text;

    OGUIButton::Sprites sprites;

    QImage upImg;
    QImage overImg;

    WOGButton* pConfig;

    bool isOver;
};

using namespace og::ui;

OGUIButton::OGUIButton()
{
    pImpl_ = new OGUIButtonImpl;

    pImpl_->pConfig = 0;
    pImpl_->isOver = false;
}

OGUIButton::OGUIButton(WOGButton* config)
{
    pImpl_ = new OGUIButtonImpl;

    pImpl_->onclick = config->onclick;
    pImpl_->text = _GetText(config->text);
    pImpl_->pConfig = config;
    pImpl_->isOver = false;

    pImpl_->sprites.upImage = 0;
    pImpl_->sprites.overImage = 0;
}

OGUIButton::~OGUIButton()
{
    delete pImpl_;
}

void OGUIButton::AddSprites(const Sprites &spritelist)
{
    pImpl_->sprites = spritelist;
}

void OGUIButton::SetOnClick(const QString &onclick)
{
    pImpl_->onclick = onclick;
}

void OGUIButton::SetUpImage(const QString &path)
{
    _SetUpImage(path);
}

void OGUIButton::SetOverImage(const QString &path)
{
    _SetOverImage(path);
}

void OGUIButton::Paint(QPainter* painter)
{
    QImage* img;

    if (pImpl_->sprites.upImage != 0)
    {
        QPixmap* pm = 0;

        if (pImpl_->isOver)
        {
            if (pImpl_->sprites.overImage != 0)
            {
                pm = &pImpl_->sprites.overImage->image;
            }
        }
        else pm = &pImpl_->sprites.upImage->image;

        if (pm) painter->drawPixmap(*this, *pm, pm->rect());

        return;
    }

    if (pImpl_->isOver) img = &pImpl_->overImg;
    else img = &pImpl_->upImg;

    if (img) { painter->drawImage(*this, *img, img->rect()); }



    QPen pen(Qt::white,  2.0f);
    painter->setOpacity(1.0f);
    painter->save();

    painter->setPen(pen);
    painter->setFont(QFont("Arial", 14, QFont::Bold));
    painter->drawText(*this, Qt::AlignCenter, pImpl_->text);

    painter->restore();
}

void OGUIButton::MouseDown(QMouseEvent* ev)
{
    Q_UNUSED(ev);
}

void OGUIButton::MouseMove(QMouseEvent* ev)
{
    Q_UNUSED(ev);

    pImpl_->isOver = true;
}

void OGUIButton::Leave(QMouseEvent* ev)
{
    Q_UNUSED(ev);

    pImpl_->isOver = false;
}

bool OGUIButton::isOver() { return pImpl_->isOver; }

inline void OGUIButton::_SetUpImage(const QString &path)
{
    pImpl_->upImg = QImage(path);
    QImage* img = &pImpl_->upImg;
    const WOGButton* config = pImpl_->pConfig;

    if (config->rotation != 0)
    {
        QTransform t;
        t.rotate(config->rotation * -1.0f);
        pImpl_->upImg = img->transformed(t);
    }

    int w = qRound(img->width() * config->scale.x());
    int h = qRound(img->height() * config->scale.y());

    setWidth(w);
    setHeight(h);

    w = og::OGGameEngine::getInstance()->getWidth();
    h = og::OGGameEngine::getInstance()->getHeight();

    int x = qRound(w * 0.5f + config->position.x());
    int y = qRound(h * 0.5f - config->position.y());

    moveCenter(QPoint(x, y));
}

inline void OGUIButton::_SetOverImage(const QString &path)
{
    pImpl_->overImg = QImage(path);
    QImage* img = &pImpl_->overImg;

    if (pImpl_->pConfig->rotation != 0)
    {
        QTransform t;
        t.rotate(pImpl_->pConfig->rotation * -1.0f);
        pImpl_->overImg = img->transformed(t);
    }
}

inline const QString &OGUIButton::_GetOnClick() const
{
    return pImpl_->onclick;
}

inline QString OGUIButton::_GetText(QString &str)
{
    return OpenGOO::instance()->GetWorld()->textdata()->GetString(str);
}
