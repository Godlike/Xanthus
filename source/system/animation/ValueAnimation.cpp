#include "system/animation/ValueAnimation.hpp"

#include "component/LifetimeComponent.hpp"

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

            valueAnimationComp.onIteration.emit(entity);

            if (now >= valueAnimationComp.endTime)
            {
                valueAnimationComp.onComplete.emit(entity);
            }
        }
        else
        {
            valueAnimationComp.onFail.emit(entity);
        }
    }
}

}
}
}
