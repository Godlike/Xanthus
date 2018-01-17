#ifndef XANTHUS_ENTITY_WORLD_HPP
#define XANTHUS_ENTITY_WORLD_HPP

#include "entity/Entity.hpp"
#include "entity/EntityPool.hpp"

#include <vector>

namespace xanthus
{
namespace entity
{

class World
{
public:
    using Entities = std::vector<Entity>;

    World() = default;

    World(World const& other) = delete;
    World& operator=(World const& other) = delete;

    ~World() = default;

    Entity CreateEntity();
    Entities CreateEntities(std::size_t count);

    void DeleteEntity(const Entity& entity);
    void DeleteEntities(Entities const& entities);

    Entities MatchEntities(component::Flags query) const;

    EntityPool& GetEntityPool() { return m_pool; }

private:
    Entities GenerateEntities(EntityPool::EntityCollection const& collection) const;
    static EntityPool::EntityCollection GenerateCollection(Entities const& entities);

    EntityPool m_pool;
};

}
}

#endif // XANTHUS_ENTITY_WORLD_HPP
