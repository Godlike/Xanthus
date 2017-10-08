#ifndef XANTHUS_SYSTEM_TIMER_HPP
#define XANTHUS_SYSTEM_TIMER_HPP

#include "WorldTime.hpp"

#include "entity/World.hpp"

#include "component/PositionComponent.hpp"
#include "component/TimerComponent.hpp"

#include "system/Skeleton.hpp"

namespace xanthus
{
namespace system
{

class Timer : public Skeleton<component::TimerComponent>
{
public:
    Timer(entity::World& world
        , WorldTime& worldTime);
    ~Timer() = default;

    void Update();

private:
    WorldTime& m_worldTime;
};

}
}

#endif // XANTHUS_SYSTEM_TIMER_HPP
