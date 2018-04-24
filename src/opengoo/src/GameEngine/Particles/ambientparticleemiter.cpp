#include "ambientparticleemiter.h"

#include "../../OGLib/util.h"
#include "../og_gameengine.h"
#include "../camera.h"

namespace og
{
template<int Precision>
class AmbientParticleEmiter::PositionGenerator
{
    AmbientParticleEmiter* mEmiter;

public:
    PositionGenerator(AmbientParticleEmiter* aEmiter)
        : mEmiter(aEmiter)
    {
    }

    QPointF GetNext(int aDegrees)
    {;
        const auto& pos = GE->GetCamera()->GetPosition();
        const auto hw = GE->GetCamera()->GetScaledWidth() * 0.5f;
        const auto hh = GE->GetCamera()->GetScaledHeight() * 0.5f;

        auto x = pos.x() + GE->GetRandomGenerator()->Range(-hw, hw);
        auto y = pos.y() + GE->GetRandomGenerator()->Range(-hh, hh);

        auto hwWithMargin = hw + mEmiter->mMargin.left();
        auto hhWithMargin = hh + mEmiter->mMargin.top();

        switch (CheckDegrees(aDegrees))
        {
        case 0:
            x = pos.x() - hwWithMargin;
            break;
        case 90:
            y = pos.y() - hhWithMargin;
            break;
        case 180:
            x = pos.x() + hwWithMargin;
            break;
        case 270:
            y = pos.y() + hhWithMargin;
            break;
        default:
            if (GE->GetRandomGenerator()->Range(0, 1))
            {
                y = (aDegrees > 0 && aDegrees < 180) ? pos.y() - hhWithMargin : pos.y() + hhWithMargin;
            }
            else
            {
                x = (aDegrees > 90 && aDegrees < 270) ? pos.x() + hwWithMargin : pos.x() - hwWithMargin;
            }
            break;
        }

        return QPointF(x, -y);
    }

private:
    static int CheckDegrees(int aDegrees)
    {
        assert(aDegrees < 360 && "GetPosition. aDegrees must be smaller than 360");

        if (IsValueInRange<90 - Precision, 90 + Precision>(aDegrees))
        {
            return 90;
        }
        else if (IsValueInRange<180 - Precision, 180 + Precision>(aDegrees))
        {
            return 180;
        }
        else if (IsValueInRange <270 - Precision, 270 + Precision>(aDegrees))
        {
            return 270;
        }
        else if (aDegrees <= 10 || aDegrees >= 350)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
};

AmbientParticleEmiter::AmbientParticleEmiter(int aMargin, int aTimeoutInterval,
                                             int aMaxParticles, ParticleSystem *aSystem)
    : ParticleEmiter(aMaxParticles, aSystem)
    , mPositionGenerator(new PositionGenerator<10>(this))
{
    SetMargin(aMargin);
    SetTimeoutInterval(aTimeoutInterval);
}

AmbientParticleEmiter::~AmbientParticleEmiter()
{
}

void AmbientParticleEmiter::PreUpdate()
{
    mTimer.Update();
    if (!mTimer.IsActive())
    {
        if (GetCountParticles() < GetMaxPirticles())
        {
            const auto i = GE->GetRandomGenerator()->Range(0, static_cast<int>(mParticleDefinations.size()) - 1);
            const auto& def = mParticleDefinations[i];
            Emit(mPositionGenerator->GetNext(def.direction), def);
        }

        mTimer.Start();
    }
}

void AmbientParticleEmiter::PostUpdate()
{
    QRectF rect(0, 0, GE->GetCamera()->GetScaledWidth(), GE->GetCamera()->GetScaledHeight());
    rect += mMargin;
    rect.moveCenter(QPointF(GE->GetCamera()->GetPosition().x(), GE->GetCamera()->GetPosition().y()));

    foreach (auto particle, mUpdate)
    {
        if (!rect.contains(QPointF(particle->GetPosition()[0], -particle->GetPosition()[1])))
        {
            particle->SetShouldRelease(true);
        }
    }
}
}
