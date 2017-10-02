#include "system/Lifetime.hpp"

#include "assemblage/Factory.hpp"

#include "entity/Entity.hpp"

#include <iostream>

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

    uint32_t count = 0;

    for (entity::Entity& entity : entities)
    {
        LifetimeComponent& component = entity.GetComponent<LifetimeComponent>();

        if (now >= component.deadline)
        {
            m_factory.ReclaimEntity(entity);
            m_world.DeleteEntity(entity);
            ++count;
        }
    }

    if (count)
    {
        std::cerr << "[Lifetime] deleted " << count << " entities" << std::endl;
    }
}

}
}
