#ifndef XANTHUS_SYSTEM_ANIMATION_VALUE_ANIMATION_HPP
#define XANTHUS_SYSTEM_ANIMATION_VALUE_ANIMATION_HPP

#include "WorldTime.hpp"

#include "entity/World.hpp"

#include "component/PositionComponent.hpp"
#include "component/ValueAnimationComponent.hpp"

#include "system/Skeleton.hpp"

namespace xanthus
{
namespace system
{
namespace animation
{

class ValueAnimation : public Skeleton<component::PositionComponent, component::ValueAnimationComponent>
{
public:
    ValueAnimation(entity::World& world
        , WorldTime& worldTime);
    ~ValueAnimation() = default;

    void Update();

private:
    WorldTime& m_worldTime;
};

}
}
}

#endif // XANTHUS_SYSTEM_ANIMATION_VALUE_ANIMATION_HPP
