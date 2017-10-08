#include "system/Timer.hpp"

namespace xanthus
{
namespace system
{

Timer::Timer(entity::World& world
    , WorldTime& worldTime
)
    : Skeleton<component::TimerComponent>(world)
    , m_worldTime(worldTime)
{

}

void Timer::Update()
{
    WorldTime::TimeUnit const now = m_worldTime.GetTime();

    entity::World::Entities entities = GetEntities();

    for (entity::Entity& entity : entities)
    {
        component::TimerComponent& timerComp = entity.GetComponent<component::TimerComponent>();

        if (now >= timerComp.endTime)
        {
            entity.DeleteComponent<component::TimerComponent>();
            continue;
        }

        if (now >= (timerComp.lastTime + timerComp.tick))
        {
            timerComp.onTick.emit(entity);
            timerComp.lastTime = now;
        }
    }
}

}
}
