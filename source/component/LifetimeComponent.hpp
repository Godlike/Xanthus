#ifndef XANTHUS_COMPONENT_LIFETIME_COMPONENT_HPP
#define XANTHUS_COMPONENT_LIFETIME_COMPONENT_HPP

#include "WorldTime.hpp"

#include "util/Types.hpp"

#include <chrono>

namespace xanthus
{
namespace component
{

struct LifetimeComponent : public Component
{
    using Unit = WorldTime::TimeUnit;

    Unit spawned;
    Unit ttl;
};

}
}

#endif // XANTHUS_COMPONENT_LIFETIME_COMPONENT_HPP
