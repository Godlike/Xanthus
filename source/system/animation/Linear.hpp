#ifndef XANTHUS_SYSTEM_ANIMATION_LINEAR_HPP
#define XANTHUS_SYSTEM_ANIMATION_LINEAR_HPP

#include "component/FollowPositionComponent.hpp"
#include "component/MoveComponent.hpp"

#include "system/animation/Filter.hpp"

#include <sleipnir/ecs/component/PositionComponent.hpp>

namespace xanthus
{
namespace system
{
namespace animation
{

void Linear(
    WorldTime::TimeUnit now
    , sleipnir::ecs::component::PositionComponent& positionComponent
    , component::FollowPositionComponent& followComponent
);

void Linear(
    WorldTime::TimeUnit now
    , sleipnir::ecs::component::PositionComponent& positionComponent
    , component::MoveComponent& followComponent
);

}
}
}

#endif // XANTHUS_SYSTEM_ANIMATION_LINEAR_HPP
