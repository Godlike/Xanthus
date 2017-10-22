#include "system/animation/Follow.hpp"

namespace xanthus
{
namespace system
{
namespace animation
{

Follow::Follow(entity::World& world
    , WorldTime& worldTime
)
    : Skeleton<component::PositionComponent, component::FollowPositionComponent>(world)
    , m_worldTime(worldTime)
{

}

void Follow::Update()
{
    WorldTime::TimeUnit now = m_worldTime.GetTime();

    entity::World::Entities entities = GetEntities();

    for (entity::Entity& entity : entities)
    {
        component::PositionComponent& posComp = entity.GetComponent<component::PositionComponent>();
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
