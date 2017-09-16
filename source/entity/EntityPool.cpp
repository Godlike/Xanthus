#include "entity/EntityPool.hpp"

#include <cassert>

namespace xanthus
{
namespace entity
{

EntityPool::EntityPool()
{
    m_entityComponents.resize(util::Config::InitialEntityCount);
    m_entityComponentFlags.resize(util::Config::InitialEntityCount);

    m_entities.reserve(util::Config::InitialEntityCount);
}

EntityId EntityPool::CreateEntity()
{
    EnsureCapacity(1);

    EntityId id = InvalidEntityId;

    if (m_freeEntities.empty())
    {
        id = m_entities.size();
        m_entities.push_back(id);
    }
    else
    {
        id = m_freeEntities.front();
        m_freeEntities.pop();
        m_entities[id] = id;
    }

    return id;
}

EntityPool::EntityCollection EntityPool::CreateEntities(std::size_t count)
{
    EntityCollection result;
    result.reserve(count);

    EnsureCapacity(count);

    std::size_t entityCount = 0;

    while (!m_freeEntities.empty() && entityCount++ < count)
    {
        const EntityId id = m_freeEntities.front();
        m_freeEntities.pop();

        result.push_back(id);
        m_entities[id] = id;
    }

    while (entityCount++ < count)
    {
        const EntityId id = m_entities.size();

        result.push_back(id);
        m_entities.push_back(id);
    }

    return result;
}

void EntityPool::DeleteEntity(EntityId entityId)
{
    assert(m_entities.size() >= entityId);
    assert(InvalidEntityId != m_entities[entityId]);

    EntityDeleteComponents(entityId);

    m_entities[entityId] = InvalidEntityId;
    m_freeEntities.push(entityId);
}

void EntityPool::DeleteEntities(EntityCollection const& entities)
{
    for (auto const& entityId : entities)
    {
        DeleteEntity(entityId);
    }
}

EntityPool::EntityCollection EntityPool::MatchEntities(component::Flags query) const
{
    EntityCollection result;

    for (EntityId const& id : m_entities)
    {
        if (InvalidEntityId != id && query.IsSubset(m_entityComponentFlags[id]))
        {
            result.push_back(id);
        }
    }

    return result;
}

bool EntityPool::EntityIsValid(EntityId entityId) const
{
    return m_entities.size() > entityId && InvalidEntityId != m_entities[entityId];
}

void EntityPool::EntityAddComponent(EntityId entityId
    , Component* pComponent
    , std::size_t componentId
)
{
    assert(m_entities.size() >= entityId);
    assert(InvalidEntityId != m_entities[entityId]);
    assert(util::Config::MaxComponentCount > componentId);

    m_entityComponents[entityId][componentId].reset(pComponent);
    m_entityComponentFlags[entityId].Set(componentId);
}

Component& EntityPool::EntityGetComponent(EntityId entityId, std::size_t componentId) const
{
    assert(m_entities.size() >= entityId);
    assert(InvalidEntityId != m_entities[entityId]);
    assert(util::Config::MaxComponentCount > componentId);

    return *m_entityComponents[entityId][componentId];
}

bool EntityPool::EntityHasComponent(EntityId entityId, std::size_t componentId) const
{
    assert(m_entities.size() >= entityId);
    assert(InvalidEntityId != m_entities[entityId]);
    assert(util::Config::MaxComponentCount > componentId);

    return m_entityComponentFlags[entityId].Test(componentId);
}

void EntityPool::EntityDeleteComponent(EntityId entityId, std::size_t componentId)
{
    assert(m_entities.size() >= entityId);
    assert(InvalidEntityId != m_entities[entityId]);
    assert(util::Config::MaxComponentCount > componentId);

    m_entityComponents[entityId][componentId].reset(nullptr);
    m_entityComponentFlags[entityId].Reset(componentId);
}

void EntityPool::EntityDeleteComponents(EntityId entityId)
{
    assert(m_entities.size() >= entityId);
    assert(InvalidEntityId != m_entities[entityId]);

    for (ComponentPtr& ptr : m_entityComponents[entityId])
    {
        ptr.reset(nullptr);
    }

    m_entityComponentFlags[entityId].Clear();
}

void EntityPool::EnsureCapacity(std::size_t count)
{
    const std::size_t size = m_entities.size();
    const std::size_t capacity = m_entities.capacity();
    const std::size_t entitiesLeft = capacity - size;

    if (entitiesLeft < count)
    {
        std::size_t newSize = capacity;

        do
        {
            newSize = static_cast<std::size_t>(static_cast<float>(newSize) * 1.61803398875f + 0.5f);
        } while ((newSize - size) < count);

        m_entityComponents.resize(newSize);
        m_entityComponentFlags.resize(newSize);

        m_entities.reserve(newSize);
    }
}

}
}
