#pragma once

#include "GameEngine/entity.h"
#include "ijointbuilder.h"

class OGSprite;

class Strand : public og::Entity
{
public:
    Strand(std::shared_ptr<OGSprite> a_sprite, IJointBuilder& a_builder)
        : Entity(0.0f, 0.0f, a_sprite)
        , m_sprite(a_sprite)
        , m_joint(a_builder.Build())
    {
    }

private:
    void Render(QPainter& a_painter);

private:
    std::shared_ptr<OGSprite> m_sprite;
    IJointBuilder::JointUPtr m_joint;
};
