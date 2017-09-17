#ifndef XANTHUS_SYSTEM_TIME_HPP
#define XANTHUS_SYSTEM_TIME_HPP

#include "entity/World.hpp"

namespace xanthus
{
namespace system
{

class Time
{
public:
    using TimeUnit = entity::World::TimeUnit;

    Time(entity::World& world);
    ~Time() = default;

    TimeUnit Update(TimeUnit realDuration);
    TimeUnit GetRealDuration() const { return m_realDuration; }
    TimeUnit GetWorldDuration() const { return m_worldDuration; }

    float factor;

private:
    TimeUnit m_realDuration;
    TimeUnit m_worldDuration;

    entity::World& m_world;
};

}
}

#endif // XANTHUS_SYSTEM_TIME_HPP
