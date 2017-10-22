#include "system/animation/Move.hpp"

namespace xanthus
{
namespace system
{
namespace animation
{

Move::Move(entity::World& world
    , WorldTime& worldTime
)
    : Skeleton<component::PositionComponent, component::MoveComponent>(world)
    , m_worldTime(worldTime)
{

}

void Move::Update()
{
    WorldTime::TimeUnit now = m_worldTime.GetTime();

    entity::World::Entities entities = GetEntities();

    for (entity::Entity& entity : entities)
    {
        component::PositionComponent& posComp = entity.GetComponent<component::PositionComponent>();
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
