#include "system/animation/Move.hpp"

namespace xanthus
{
namespace system
{
namespace animation
{

Move::Move(sleipnir::ecs::entity::World& world
    , WorldTime& worldTime
)
    : Skeleton<sleipnir::ecs::component::PositionComponent, component::MoveComponent>(world)
    , m_worldTime(worldTime)
{

}

void Move::Update()
{
    WorldTime::TimeUnit now = m_worldTime.GetTime();

    sleipnir::ecs::entity::World::Entities entities = GetEntities();

    for (sleipnir::ecs::entity::Entity& entity : entities)
    {
        sleipnir::ecs::component::PositionComponent& posComp = entity.GetComponent<sleipnir::ecs::component::PositionComponent>();
        component::MoveComponent& moveComp = entity.GetComponent<component::MoveComponent>();

        moveComp.pFilter(now, posComp, moveComp);

        if (posComp.position == moveComp.targetPosition)
        {
            moveComp.onComplete.emit(entity);
            entity.DeleteComponent<component::MoveComponent>();
        }
    }
}

}
}
}
