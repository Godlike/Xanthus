#ifndef XANTHUS_SYSTEM_ANIMATION_MOVE_HPP
#define XANTHUS_SYSTEM_ANIMATION_MOVE_HPP

#include "WorldTime.hpp"

#include "entity/World.hpp"

#include "component/PositionComponent.hpp"
#include "component/MoveComponent.hpp"

#include "system/Skeleton.hpp"

namespace xanthus
{
namespace system
{
namespace animation
{

class Move : public Skeleton<component::PositionComponent, component::MoveComponent>
{
public:
    Move(entity::World& world
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
