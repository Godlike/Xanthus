#ifndef XANTHUS_SYSTEM_ANIMATION_FILTER_HPP
#define XANTHUS_SYSTEM_ANIMATION_FILTER_HPP

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

typedef void (*FilterFunc)(
    WorldTime::TimeUnit now
    , component::PositionComponent& positionComponent
    , component::FollowPositionComponent& followComponent
);

typedef void (*MoveFilterFunc)(
    WorldTime::TimeUnit now
    , component::PositionComponent& positionComponent
    , component::MoveComponent& moveComponent
);

}
}
}

#endif // XANTHUS_SYSTEM_ANIMATION_FILTER_HPP
