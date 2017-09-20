#include "system/Time.hpp"

#include <iostream>

namespace xanthus
{
namespace system
{

Time::Time(entity::World& world)
    : factor(0.1f)
    , m_realDuration(0)
    , m_worldDuration(0)
    , m_world(world)
{

}

Time::TimeUnit Time::Update(TimeUnit realDuration)
{
    m_realDuration = realDuration;
    m_worldDuration = TimeUnit(static_cast<uint64_t>(static_cast<float>(realDuration.count()) * factor + 0.5f));

    m_world.SetTime(m_world.GetTime() + m_worldDuration);

    return m_worldDuration;
}

}
}
