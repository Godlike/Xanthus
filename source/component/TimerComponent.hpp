#ifndef XANTHUS_COMPONENT_TIMER_COMPONENT_HPP
#define XANTHUS_COMPONENT_TIMER_COMPONENT_HPP

#include "WorldTime.hpp"

#include "entity/Entity.hpp"

#include "util/Types.hpp"

#include <wink/signal.hpp>

namespace xanthus
{
namespace component
{

struct TimerComponent : public Component
{
    WorldTime::TimeUnit lastTime;
    WorldTime::TimeUnit endTime;

    WorldTime::TimeUnit tick;

    wink::signal< wink::slot<void(entity::Entity)> > onTick;
};

}
}

#endif // XANTHUS_COMPONENT_TIMER_COMPONENT_HPP
