#include "og_sprite.h"

void OGSprite::Paint(QPainter* p)
{
    if (!m_visible)
        return;

    QPointF target(-m_offsetX, -m_offsetY);
    auto sx = m_scale * m_scaleX;
    auto sy = m_scale * m_scaleY;

    p->save();
    p->translate(GetX(), GetY());

    if (GetAngle() != 0)
        p->rotate(GetAngle());

    p->scale(sx, sy);
    p->setOpacity(m_alpha);
    m_source->Render(*p, target, m_clipRect);
    p->restore();
}

void OGSprite::Paint(QPainter& p, const QRectF& a_target)
{
    if (!m_visible)
        return;

    p.setOpacity(m_alpha);

    p.resetTransform();

    if (GetAngle() != 0)
        p.rotate(GetAngle());

    m_source->Render(p, a_target, m_clipRect);
}

void OGSprite::Render(QPainter& a_painter, const QVector2D& a_pos)
{
    QPointF target(-m_offsetX, -m_offsetY);
    auto sx = m_scale * m_scaleX;
    auto sy = m_scale * m_scaleY;

    a_painter.save();
    a_painter.translate(a_pos.x(), a_pos.y());

    if (GetAngle() != 0)
        a_painter.rotate(GetAngle());

    a_painter.scale(sx, sy);
    a_painter.setOpacity(m_alpha);

    m_source->Render(a_painter, target, m_clipRect);

    a_painter.restore();
}

void OGSprite::CenterOrigin()
{
    m_offsetX = m_source->GetWidth() / 2.0f;
    m_offsetY = m_source->GetHeight() / 2.0f;
}
