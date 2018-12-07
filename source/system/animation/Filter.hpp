#ifndef XANTHUS_SYSTEM_ANIMATION_FILTER_HPP
#define XANTHUS_SYSTEM_ANIMATION_FILTER_HPP

#include <sleipnir/ecs/component/PositionComponent.hpp>
#include <sleipnir/ecs/WorldTime.hpp>

namespace xanthus
{

namespace component
{

struct FollowPositionComponent;
struct MoveComponent;
struct PositionComponent;

}

namespace system
{
namespace animation
{

using WorldTime = sleipnir::ecs::WorldTime;

typedef void (*FilterFunc)(
    WorldTime::TimeUnit now
    , sleipnir::ecs::component::PositionComponent& positionComponent
    , component::FollowPositionComponent& followComponent
);

typedef void (*MoveFilterFunc)(
    WorldTime::TimeUnit now
    , sleipnir::ecs::component::PositionComponent& positionComponent
    , component::MoveComponent& moveComponent
);

}
}
}

#endif // XANTHUS_SYSTEM_ANIMATION_FILTER_HPP
