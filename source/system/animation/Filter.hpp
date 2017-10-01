#ifndef XANTHUS_SYSTEM_ANIMATION_FILTER_HPP
#define XANTHUS_SYSTEM_ANIMATION_FILTER_HPP

namespace xanthus
{

namespace component
{

struct PositionComponent;
struct ValueAnimationComponent;

}

namespace system
{
namespace animation
{

typedef void (*FilterFunc)(
    WorldTime::TimeUnit now
    , component::PositionComponent& positionComponent
    , component::ValueAnimationComponent& animationComponent
);

}
}
}

#endif // XANTHUS_SYSTEM_ANIMATION_FILTER_HPP
