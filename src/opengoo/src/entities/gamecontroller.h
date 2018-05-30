#pragma once

#include <list>

#include "GameEngine/entity.h"

#include "SoundEngine/sound.h"
#include "SoundEngine/music.h"

class Animator;
class Ball;
struct WOGBall;

class GameController : public og::Entity
{
    enum State
    {
        e_finding_marker,
        e_marker_is_found,
        e_marker_is_dragging
    };

public:
    GameController();

    void SetMusic(og::audio::Music* aMusic)
    {
        mMusic = aMusic;
    }

    void AddLoopSound(SoundSPtr aSound)
    {
        mLoopSounds.push_back(aSound);
    }

    void AddBall(std::shared_ptr<Ball> aBall);

    void SetExitPosition(const QPointF& aPosition)
    {
        mExitPosition = QVector2D(aPosition.x(), -aPosition.y());
    }

private:
    void Update();

    void Added();

    void Removed();

    void Render(QPainter& a_painter);

    void FillNearsBalls(int aNum, int aLen, const QVector2D& aPos);

    void ResetMarker();

    void MarkBall(std::shared_ptr<Ball> &aBall);

    void UnMarkBall();

    void AttachMarker();

    void CreateStrands();

    std::list<std::shared_ptr<Ball>>::iterator FindMarker(const QVector2D& aPosition);

private:
    og::audio::Music* mMusic;
    std::list<SoundSPtr> mLoopSounds;
    std::list<std::shared_ptr<Ball>> mBall;
    std::list<std::shared_ptr<Ball>> mAttachedBall;
    std::shared_ptr<Ball> mMarker;
    const WOGBall* mBallDef;
    std::vector<std::reference_wrapper<std::shared_ptr<Ball>>> mNearestBalls;
    State mState;
    Ball* mNearestBall;
    QVector2D mExitPosition;
};
