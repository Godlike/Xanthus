#ifndef XANTHUS_SYSTEM_ANIMATION_LINEAR_HPP
#define XANTHUS_SYSTEM_ANIMATION_LINEAR_HPP

#include "component/PositionComponent.hpp"
#include "component/FollowPositionComponent.hpp"
#include "component/MoveComponent.hpp"

#include "system/animation/Filter.hpp"

namespace xanthus
{
namespace system
{
namespace animation
{

void Linear(
    WorldTime::TimeUnit now
    , component::PositionComponent& positionComponent
    , component::FollowPositionComponent& followComponent
);

void Linear(
    WorldTime::TimeUnit now
    , component::PositionComponent& positionComponent
    , component::MoveComponent& followComponent
);

}
}
}

#endif // XANTHUS_SYSTEM_ANIMATION_LINEAR_HPP
