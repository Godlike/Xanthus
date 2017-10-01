#include "system/Time.hpp"

#include "util/Config.hpp"

#include <iostream>

namespace xanthus
{
namespace system
{

Time::Time(WorldTime& worldTime
    , physics::Physics& physicsSystem
)
    : factor(0.1f)
    , m_realDuration(0)
    , m_worldDuration(0)
    , m_worldTime(worldTime)
    , m_physicsSystem(physicsSystem)
{

}

Time::TimeUnit Time::Update(TimeUnit realDuration)
{
    m_realDuration = realDuration;

    TimeUnit const worldNow = m_worldTime.GetTime();
    TimeUnit const physNow = m_physicsSystem.GetCurrentTime();

    if ((worldNow - physNow) > util::Config::PhysicsTick)
    {
        m_worldDuration = TimeUnit(0);
    }
    else
    {
        m_worldDuration = TimeUnit(static_cast<uint64_t>(static_cast<float>(realDuration.count()) * factor + 0.5f));
    }

    m_worldTime.SetTime(worldNow + m_worldDuration);

    return m_worldDuration;
}

}
}
