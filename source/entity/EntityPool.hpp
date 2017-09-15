#ifndef XANTHUS_ENTITY_ENTITY_POOL_HPP
#define XANTHUS_ENTITY_ENTITY_POOL_HPP

#include "component/Flags.hpp"

#include "util/Config.hpp"
#include "util/Types.hpp"

#include <vector>
#include <queue>

namespace xanthus
{
namespace entity
{

class EntityPool
{
public:
    using EntityCollection = std::vector<EntityId>;

    EntityPool();
    ~EntityPool() = default;

    EntityId CreateEntity();
    EntityCollection CreateEntities(std::size_t count);
    void DeleteEntity(EntityId entity);
    void DeleteEntities(EntityCollection const& entities);

    EntityCollection MatchEntities(component::Flags query) const;

    bool EntityIsValid(EntityId entity) const;

    void EntityAddComponent(EntityId entityId,
        Component* pComponent,
        std::size_t componentId);

    Component& EntityGetComponent(EntityId entityId, std::size_t componentId) const;

    bool EntityHasComponent(EntityId entityId, std::size_t componentId) const;

    void EntityDeleteComponent(EntityId entityId, std::size_t componentId);
    void EntityDeleteComponents(EntityId entityId);

private:
    typedef std::array<ComponentPtr, util::Config::MaxComponentCount> ComponentArray;

    void EnsureCapacity(std::size_t count);

    //! Holds components for all entities
    std::vector<ComponentArray> m_entityComponents;

    //! Holds component flags for all entities
    std::vector<component::Flags> m_entityComponentFlags;

    //! Holds all alive entities
    EntityCollection m_entities;

    //! Holds all free entities
    std::queue<EntityId> m_freeEntities;
};

}
}

#endif // XANTHUS_ENTITY_ENTITY_POOL_HPP
