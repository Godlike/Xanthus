#include "entity/World.hpp"

namespace xanthus
{
namespace entity
{

Entity World::CreateEntity()
{
    return Entity(this, m_pool.CreateEntity());
}

void World::DeleteEntity(const Entity& entity)
{
    m_pool.DeleteEntity(entity.GetId());
}

}
}
