#ifndef XANTHUS_SYSTEM_ANIMATION_FOLLOW_HPP
#define XANTHUS_SYSTEM_ANIMATION_FOLLOW_HPP

#include "component/FollowPositionComponent.hpp"

#include <sleipnir/ecs/component/PositionComponent.hpp>
#include <sleipnir/ecs/entity/World.hpp>
#include <sleipnir/ecs/system/Skeleton.hpp>
#include <sleipnir/ecs/WorldTime.hpp>

namespace xanthus
{
namespace system
{
namespace animation
{

class Follow
    : public sleipnir::ecs::system::Skeleton<sleipnir::ecs::component::PositionComponent, component::FollowPositionComponent>
{
public:
    using WorldTime = sleipnir::ecs::WorldTime;

    Follow(sleipnir::ecs::entity::World& world
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
