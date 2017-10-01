#include "system/Lifetime.hpp"

#include "assemblage/Factory.hpp"

#include "entity/Entity.hpp"

namespace xanthus
{
namespace system
{

Lifetime::Lifetime(entity::World& world
    , WorldTime& worldTime
    , assemblage::Factory& factory
)
    : Skeleton<component::LifetimeComponent>(world)
    , m_worldTime(worldTime)
    , m_factory(factory)
{

}

void Lifetime::Update()
{
    using component::LifetimeComponent;

    entity::World::Entities entities = GetEntities();
    WorldTime::TimeUnit const now = m_worldTime.GetTime();

    for (entity::Entity& entity : entities)
    {
        LifetimeComponent& component = entity.GetComponent<LifetimeComponent>();

        if (now > (component.spawned + component.ttl))
        {
            m_factory.ReclaimEntity(entity);
            m_world.DeleteEntity(entity);
        }
    }
}

}
}
