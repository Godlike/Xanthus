#include "entity/World.hpp"

#include <algorithm>

namespace xanthus
{
namespace entity
{

Entity World::CreateEntity()
{
    return Entity(this, m_pool.CreateEntity());
}

World::Entities World::CreateEntities(std::size_t count)
{
    return GenerateEntities(m_pool.CreateEntities(count));
}

void World::DeleteEntity(const Entity& entity)
{
    m_pool.DeleteEntity(entity.GetId());
}

void World::DeleteEntities(Entities const& entities)
{
    m_pool.DeleteEntities(GenerateCollection(entities));
}

World::Entities World::MatchEntities(component::Flags query) const
{
    return GenerateEntities(m_pool.MatchEntities(query));
}

World::Entities World::GenerateEntities(EntityPool::EntityCollection const& collection) const
{
    World::Entities entities;
    entities.reserve(collection.size());

    std::transform(collection.begin(), collection.end(), std::back_inserter(entities),
        [this](EntityId id) -> Entity
        {
            return Entity(const_cast<World*>(this), id);
        }
    );

    return entities;
}

EntityPool::EntityCollection World::GenerateCollection(Entities const& entities)
{
    EntityPool::EntityCollection collection;
    collection.reserve(entities.size());

    std::transform(entities.begin(), entities.end(), std::back_inserter(collection),
        [](Entity const& entity)
        {
            return entity.GetId();
        }
    );

    return collection;
}

}
}
