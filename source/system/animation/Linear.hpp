#ifndef XANTHUS_SYSTEM_ANIMATION_LINEAR_HPP
#define XANTHUS_SYSTEM_ANIMATION_LINEAR_HPP

#include "component/PositionComponent.hpp"
#include "component/ValueAnimationComponent.hpp"

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
    , component::ValueAnimationComponent& animationComponent
);

}
}
}

#endif // XANTHUS_SYSTEM_ANIMATION_LINEAR_HPP
