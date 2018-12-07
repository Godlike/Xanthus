#ifndef XANTHUS_SYSTEM_ANIMATION_MOVE_HPP
#define XANTHUS_SYSTEM_ANIMATION_MOVE_HPP

#include "component/MoveComponent.hpp"

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

class Move
    : public sleipnir::ecs::system::Skeleton<sleipnir::ecs::component::PositionComponent, component::MoveComponent>
{
public:
    using WorldTime = sleipnir::ecs::WorldTime;

    Move(sleipnir::ecs::entity::World& world
        , WorldTime& worldTime);
    ~Move() = default;

    void Update();

private:
    WorldTime& m_worldTime;
};

}
}
}

#endif // XANTHUS_SYSTEM_ANIMATION_MOVE_HPP
