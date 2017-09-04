#include "entity/Entity.hpp"

#include "entity/World.hpp"

#include <cassert>

namespace xanthus
{
namespace entity
{

Entity::Entity()
    : m_id(InvalidEntityId)
    , m_pWorld(nullptr)
{

}

Entity::Entity(World* world, EntityId id)
    : m_id(id)
    , m_pWorld(world)
{
    assert(nullptr != m_pWorld);
}

bool Entity::operator==(const Entity& other) const
{
    return m_id == other.m_id
        && m_pWorld == other.m_pWorld
    ;
}

bool Entity::IsValid() const
{
    return nullptr != m_pWorld && m_pWorld->GetEntityPool().EntityIsValid(m_id);
}

void Entity::Destroy()
{
    m_pWorld->GetEntityPool().DeleteEntity(m_id);
}

void Entity::AddComponentImpl(Component* pComponent, std::size_t componentId)
{
    m_pWorld->GetEntityPool().EntityAddComponent(m_id, pComponent, componentId);
}

Component& Entity::GetComponentImpl(std::size_t componentId) const
{
    return m_pWorld->GetEntityPool().EntityGetComponent(m_id, componentId);
}

bool Entity::HasComponentImpl(std::size_t componentId) const
{
    return m_pWorld->GetEntityPool().EntityHasComponent(m_id, componentId);
}

void Entity::DeleteComponentImpl(std::size_t componentId)
{
    m_pWorld->GetEntityPool().EntityDeleteComponent(m_id, componentId);
}

}
}
