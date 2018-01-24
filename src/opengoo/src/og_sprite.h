#pragma once

#include <memory>

#include <QPainter>
#include <QVector2D>

#include "wog_vobject.h"

#include "GameEngine/imagesource.h"
#include "GameEngine/graphic.h"

class QPainter;

class OGSprite : public og::Graphic
{    
    QVector2D m_position;
    og::ImageSourceSPtr m_source;
    float m_angle;
    float m_offsetX;
    float m_offsetY;
    bool m_visible;
    float m_scaleX;
    float m_scaleY;
    float m_alpha;
    float m_depth;
    QRectF m_clipRect;
    QColor m_colorize;

    void Init()
    {
        m_visible = true;
        m_angle = 0.0f;
        m_offsetX = 0.0f;
        m_offsetY = 0.0f;
        m_scaleX = 1.0f;
        m_scaleY = 1.0f;
        m_alpha = 1.0f;
        m_depth = 0.0f;
        m_clipRect = QRectF(0, 0, GetWidth(), GetHeight());
    }

public:
    typedef std::shared_ptr<OGSprite> OGSpritePtr;

    OGSprite()
    {
        Init();
    }

    OGSprite(og::ImageSourceSPtr a_source)
        : m_source(a_source)
    {
        Init();
    }

    OGSprite(float a_x, float a_y, og::ImageSourceSPtr a_source)
        : m_position(a_x, a_y)
        , m_source(a_source)
    {
        Init();
    }

    void Paint(QPainter* p);

    void Paint(QPainter& p, const QRectF& a_target);

    void Render(QPainter& a_painter, const QVector2D& a_pos);

    void Update()
    {
    }

    void SetAngle(float a_angle)
    {
        m_angle = a_angle;
    }

    float GetAngle() const
    {
        return m_angle;
    }

    void CenterOrigin();

    float GetX() const
    {
        return m_position.x();
    }

    float GetY() const
    {
        return m_position.y();
    }

    void SetX(float a_x)
    {
        m_position.setX(a_x);
    }

    void SetY(float a_y)
    {
        m_position.setY(a_y);
    }

    void SetVisible(bool a_visible)
    {
        m_visible = a_visible;
    }

    void SetPosition(float a_x, float a_y)
    {
        m_position.setX(a_x);
        m_position.setY(a_y);
    }

    const QVector2D& GetPosition() const
    {
        return m_position;
    }

    int GetWidth() const
    {
        return m_source->GetWidth();
    }

    int GetHeight() const
    {
        return m_source->GetHeight();
    }

    void SetScaleX(float a_scale)
    {
        m_scaleX = a_scale;
    }

    void SetScaleY(float a_scale)
    {
        m_scaleY = a_scale;
    }

    void SetScale(const QPointF& a_scale)
    {
        SetScaleX(a_scale.x());
        SetScaleY(a_scale.y());
    }

    float GetScaleX() const
    {
        return m_scaleX;
    }

    float GetScaleY() const
    {
        return m_scaleY;
    }

    float GetScaledWidth() const
    {
        return GetWidth() * m_scaleX;
    }

    float GetScaledHeight() const
    {
        return GetHeight() * m_scaleY;
    }

    void SetAlpha(float a_alpha)
    {
        m_alpha = a_alpha;
    }

    float GetAlpha() const
    {
        return m_alpha;
    }

    void SetDepth(float a_depth)
    {
        m_depth = a_depth;
    }

    float GetDepth() const
    {
        return m_depth;
    }

    void SetOffsetX(float a_offset)
    {
        m_offsetX = a_offset;
    }

    void SetOffsetY(float a_offset)
    {
        m_offsetY = a_offset;
    }

    void SetColorize(const QColor& a_color)
    {
       m_colorize = a_color;
    }

    static OGSpritePtr Create(og::ImageSourceSPtr a_source)
    {
        return std::make_shared<OGSprite>(a_source);
    }
};
