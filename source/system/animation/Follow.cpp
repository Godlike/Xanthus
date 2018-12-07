#include "system/animation/Follow.hpp"

namespace xanthus
{
namespace system
{
namespace animation
{

Follow::Follow(sleipnir::ecs::entity::World& world
    , WorldTime& worldTime
)
    : Skeleton<sleipnir::ecs::component::PositionComponent, component::FollowPositionComponent>(world)
    , m_worldTime(worldTime)
{

}

void Follow::Update()
{
    WorldTime::TimeUnit now = m_worldTime.GetTime();

    sleipnir::ecs::entity::World::Entities entities = GetEntities();

    for (sleipnir::ecs::entity::Entity& entity : entities)
    {
        sleipnir::ecs::component::PositionComponent& posComp = entity.GetComponent<sleipnir::ecs::component::PositionComponent>();
        component::FollowPositionComponent& followComp = entity.GetComponent<component::FollowPositionComponent>();

        if (followComp.target.IsValid())
        {
            followComp.pFilter(now, posComp, followComp);

            if (now >= followComp.endTime)
            {
                followComp.onComplete.emit(entity);
                entity.DeleteComponent<component::FollowPositionComponent>();
            }
        }
        else
        {
            followComp.onFail.emit(entity);
            entity.DeleteComponent<component::FollowPositionComponent>();
        }
    }
}

}
}
}
