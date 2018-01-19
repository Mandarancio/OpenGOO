#include "ambientparticleemiter.h"

#include "../../OGLib/util.h"
#include "../og_gameengine.h"
#include "../camera.h"

namespace og
{
template<int Precision>
class AmbientParticleEmiter::PositionGenerator
{
public:
    static QPointF GetNext(int aDegrees)
    {;
        const auto& pos = GE->getCamera()->GetPosition();
        const auto hw = GE->getCamera()->GetScaledWidth() * 0.5f;
        const auto hh = GE->getCamera()->GetScaledHeight() * 0.5f;

        auto x = pos.x() + GE->GetRandomGenerator()->Range(-hw, hw);
        auto y = pos.y() + GE->GetRandomGenerator()->Range(-hh, hh);

        switch (CheckDegrees(aDegrees))
        {
        case 0:
            x = pos.x() - hw;
            break;
        case 90:
            y = pos.y() - hh;
            break;
        case 180:
            x = pos.x() + hw;
            break;
        case 270:
            y = pos.y() + hh;
            break;
        default:
            if (GE->GetRandomGenerator()->Range(0, 1))
            {
                y = (aDegrees > 0 && aDegrees < 180) ? pos.y() - hh : pos.y() + hh;
            }
            else
            {
                x = (aDegrees > 90 && aDegrees < 270) ? pos.x() + hw : pos.x() - hw;
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

void AmbientParticleEmiter::Update()
{
    const static int Precision = 10;

    mTimer.Update();
    if (!mTimer.IsActive())
    {
        if (GetCountParticles() < GetMaxPirticles())
        {
            const auto i = GE->GetRandomGenerator()->Range(0, static_cast<int>(mParticleDefinations.size()) - 1);
            const auto& def = mParticleDefinations[i];
            Emit(PositionGenerator<Precision>::GetNext(def.direction), def);
        }

        mTimer.Start();
    }

    ParticleEmiter::Update();

    QRectF rect(0, 0, GE->getCamera()->GetScaledWidth(), GE->getCamera()->GetScaledHeight());
    rect += mMargin;
    rect.moveCenter(QPointF(GE->getCamera()->GetPosition().x(), GE->getCamera()->GetPosition().y()));

    foreach (auto particle, mUpdate)
    {
        if (!rect.contains(QPointF(particle->GetPosition()[0], -particle->GetPosition()[1])))
        {
            particle->SetShouldRelease(true);
        }
    }
}
}
