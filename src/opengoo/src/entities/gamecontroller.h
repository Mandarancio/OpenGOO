#pragma once

#include <list>

#include "GameEngine/entity.h"

#include "SoundEngine/sound.h"
#include "SoundEngine/music.h"

#include "exiteventlistener.h"

class Animator;
class Ball;
struct WOGBall;

class GameController : public og::Entity, public ExitEventListener
{
    enum State
    {
        e_finding_marker,
        e_marker_is_found,
        e_marker_is_dragging
    };

    class Exit;

    struct Deleter
    {
        void operator()(Exit* aPtr) const;
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

    void InitExit(const QPointF& aPosition, float aRadius, float aRatio);

    void SetBallsRequired(int aNum)
    {
        mBallsRequired = aNum;
    }

private:
    void Update();

    void Added();

    void Removed();

    void Render(QPainter& a_painter);

    void FillNearsAttachedBalls(int aNum, int aLen, const QVector2D& aPos);

    void ResetMarker();

    void MarkBall(std::shared_ptr<Ball> &aBall);

    void UnMarkBall();

    void AttachMarker();

    void CreateStrands();

    std::vector<std::shared_ptr<Ball>>::iterator FindMarker(const QVector2D& aPosition);

    void OnOpen()
    {
        mShouldSuckingBalls = true;
    }

    void OnClosed()
    {
        mShouldSuckingBalls = false;
    }

    EntityPtr CreateContinueButton();

private:
    og::audio::Music* mMusic;
    std::list<SoundSPtr> mLoopSounds;
    std::vector<std::shared_ptr<Ball>> mBall;
    std::list<std::shared_ptr<Ball>> mAttachedBall;
    std::shared_ptr<Ball> mMarker;
    const WOGBall* mBallDef;
    std::vector<std::reference_wrapper<std::shared_ptr<Ball>>> mNearestAttachedBalls;
    State mState;
    Ball* mNearestBall;
    QVector2D mExitPosition;
    bool mShouldSuckingBalls;
    std::unique_ptr<Exit, Deleter> mExit;
    int mBallsRequired;
    bool mIsWon;
};
