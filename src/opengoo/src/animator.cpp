#include "animator.h"

#include "opengoo.h"

void Animator::Update()
{
    foreach (auto& group, mAnimationGroup)
    {
        group->Update(GAME->GetDeltaTime());
    }
}
