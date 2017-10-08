#ifndef XANTHUS_SYSTEM_ANIMATION_FOLLOW_ANIMATION_HPP
#define XANTHUS_SYSTEM_ANIMATION_FOLLOW_ANIMATION_HPP

#include "WorldTime.hpp"

#include "entity/World.hpp"

#include "component/PositionComponent.hpp"
#include "component/FollowPositionComponent.hpp"

#include "system/Skeleton.hpp"

namespace xanthus
{
namespace system
{
namespace animation
{

class FollowAnimation : public Skeleton<component::PositionComponent, component::FollowPositionComponent>
{
public:
    FollowAnimation(entity::World& world
        , WorldTime& worldTime);
    ~FollowAnimation() = default;

    void Update();

private:
    WorldTime& m_worldTime;
};

}
}
}

#endif // XANTHUS_SYSTEM_ANIMATION_FOLLOW_ANIMATION_HPP
