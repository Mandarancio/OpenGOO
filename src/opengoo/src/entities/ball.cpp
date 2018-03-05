#include "GameEngine/input.h"
#include "GameEngine/og_gameengine.h"

#include "scene.h"
#include "ball.h"

void Ball::Update()
{
    og::physics::Shape::Transform t = {GetPhysicsBody()->GetPosition(), 0};

    auto pos = QVector2D(GE->windowToLogical(og::MouseInput::GetPosition()));
    pos *= GetScene()->GetPhysicsEngine()->GetRatio();

    if (GetPhysicsBody()->GetShape()->TestPoint(t, pos))
    {
        SetVisible(false);
    }
    else
    {
        SetVisible(true);
    }
}
