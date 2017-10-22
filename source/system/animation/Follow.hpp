#ifndef XANTHUS_SYSTEM_ANIMATION_FOLLOW_HPP
#define XANTHUS_SYSTEM_ANIMATION_FOLLOW_HPP

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

class Follow : public Skeleton<component::PositionComponent, component::FollowPositionComponent>
{
public:
    Follow(entity::World& world
        , WorldTime& worldTime);
    ~Follow() = default;

    void Update();

private:
    WorldTime& m_worldTime;
};

}
}
}

#endif // XANTHUS_SYSTEM_ANIMATION_FOLLOW_HPP
