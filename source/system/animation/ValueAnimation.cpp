#include "system/animation/ValueAnimation.hpp"

namespace xanthus
{
namespace system
{
namespace animation
{

ValueAnimation::ValueAnimation(entity::World& world
    , WorldTime& worldTime
)
    : Skeleton<component::PositionComponent, component::ValueAnimationComponent>(world)
    , m_worldTime(worldTime)
{

}

void ValueAnimation::Update()
{
    WorldTime::TimeUnit now = m_worldTime.GetTime();

    entity::World::Entities entities = GetEntities();

    for (entity::Entity& entity : entities)
    {
        component::PositionComponent& posComp = entity.GetComponent<component::PositionComponent>();
        component::ValueAnimationComponent& valueAnimationComp = entity.GetComponent<component::ValueAnimationComponent>();

        if (valueAnimationComp.target.IsValid())
        {
            valueAnimationComp.pFilter(now, posComp, valueAnimationComp);

            if (now >= valueAnimationComp.endTime)
            {
                valueAnimationComp.onComplete.emit(entity);
                entity.DeleteComponent<component::ValueAnimationComponent>();
            }
        }
        else
        {
            valueAnimationComp.onFail.emit(entity);
            entity.DeleteComponent<component::ValueAnimationComponent>();
        }
    }
}

}
}
}
