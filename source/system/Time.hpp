#ifndef XANTHUS_SYSTEM_TIME_HPP
#define XANTHUS_SYSTEM_TIME_HPP

#include "WorldTime.hpp"

#include "system/physics/Physics.hpp"

namespace xanthus
{
namespace system
{

class Time
{
public:
    using TimeUnit = WorldTime::TimeUnit;

    Time(WorldTime& worldTime
        , physics::Physics& physicsSystem
    );
    ~Time() = default;

    TimeUnit Update(TimeUnit realDuration);
    TimeUnit GetRealDuration() const { return m_realDuration; }
    TimeUnit GetWorldDuration() const { return m_worldDuration; }

    float factor;

private:
    TimeUnit m_realDuration;
    TimeUnit m_worldDuration;

    WorldTime& m_worldTime;
    physics::Physics& m_physicsSystem;
};

}
}

#endif // XANTHUS_SYSTEM_TIME_HPP
