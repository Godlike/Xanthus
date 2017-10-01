#include "system/Time.hpp"

#include <iostream>

namespace xanthus
{
namespace system
{

Time::Time(WorldTime& worldTime)
    : factor(0.1f)
    , m_realDuration(0)
    , m_worldDuration(0)
    , m_worldTime(worldTime)
{

}

Time::TimeUnit Time::Update(TimeUnit realDuration)
{
    m_realDuration = realDuration;
    m_worldDuration = TimeUnit(static_cast<uint64_t>(static_cast<float>(realDuration.count()) * factor + 0.5f));

    m_worldTime.SetTime(m_worldTime.GetTime() + m_worldDuration);

    return m_worldDuration;
}

}
}
