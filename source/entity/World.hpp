#ifndef XANTHUS_ENTITY_WORLD_HPP
#define XANTHUS_ENTITY_WORLD_HPP

#include "entity/Entity.hpp"
#include "entity/EntityPool.hpp"

#include "util/Config.hpp"

#include <map>

namespace xanthus
{
namespace entity
{

class Entity;

class World
{
public:
    World() = default;
    ~World() = default;

    Entity CreateEntity();
    void DeleteEntity(const Entity& entity);

    EntityPool& GetEntityPool() { return m_pool; }

private:
    EntityPool m_pool;
};

}
}

#endif // XANTHUS_ENTITY_WORLD_HPP
