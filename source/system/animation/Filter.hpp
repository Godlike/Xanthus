#ifndef XANTHUS_SYSTEM_ANIMATION_FILTER_HPP
#define XANTHUS_SYSTEM_ANIMATION_FILTER_HPP

namespace xanthus
{

namespace component
{

struct PositionComponent;
struct FollowPositionComponent;

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

}
}
}

#endif // XANTHUS_SYSTEM_ANIMATION_FILTER_HPP
