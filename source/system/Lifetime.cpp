#include "system/Lifetime.hpp"

#include "entity/Entity.hpp"

namespace xanthus
{
namespace system
{

Lifetime::Lifetime(entity::World& world)
    : Skeleton<component::LifetimeComponent>(world)
{

}

void Lifetime::Update()
{
    using component::LifetimeComponent;

    entity::World::Entities entities = GetEntities();
    const TimeUnit now = m_world.GetTime();

    for (entity::Entity& entity : entities)
    {
        LifetimeComponent& component = entity.GetComponent<LifetimeComponent>();

        if (now > (component.spawned + component.ttl))
        {
            m_world.DeleteEntity(entity);
        }
    }
}

}
}
