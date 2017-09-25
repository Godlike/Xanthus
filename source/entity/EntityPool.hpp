#ifndef XANTHUS_ENTITY_ENTITY_POOL_HPP
#define XANTHUS_ENTITY_ENTITY_POOL_HPP

#include "component/Flags.hpp"

#include "util/Config.hpp"
#include "util/Types.hpp"

#include <array>
#include <queue>
#include <vector>

namespace xanthus
{
namespace entity
{

class EntityPool
{
public:
    using EntityCollection = std::vector<EntityId>;

    /** @brief  Creates entity pool
     *
     *  Allocates enough space to hold `util::Config::InitialEntityCount` numbber of entities
     */
    EntityPool();
    ~EntityPool() = default;

    /** @brief  Returns new entity id
     *
     *  First checks if there are invalid ids to hand out
     *  If no free ids are available, creates a fresh one
     *
     *  @return new valid entity id
     */
    EntityId CreateEntity();

    /** @brief  Returns a collection of valid entity ids
     *
     *  Checks if there are invalid ids to hand out. If more ids are needed, creates fresh ones
     *
     *  @return a collection of valid entity ids
     */
    EntityCollection CreateEntities(std::size_t count);

    /** @brief  Invalidates given entity
     *
     *  @param  entity  valid entity id
     */
    void DeleteEntity(EntityId entity);

    /** @brief  Invalidates given collection of entities
     *
     *  @param  entities    collection of valid entity ids
     */
    void DeleteEntities(EntityCollection const& entities);

    /** @brief  Searches entities that match given flags
     *
     *  Prepares a collection of valid entity ids that contain given collection of component ids
     *
     *  @param  query   collection of flags corresponding to required component ids
     *
     *  @return a collection of valid entity ids
     */
    EntityCollection MatchEntities(component::Flags query) const;

    /** @brief  Checks if given entity id is valid
     *
     *  @return @c true if entity is valid, @c false otherwise
     */
    bool EntityIsValid(EntityId entity) const;

    /** @brief  Registers given component with given entity
     *
     *  @attention  EntityPool assumes control over a lifetime of given @p pComponent
     *
     *  @param  entityId    valid entity id
     *  @param  pComponent  pointer to component
     *  @param  componentId id of given component
     */
    void EntityAddComponent(EntityId entityId,
        Component* pComponent,
        std::size_t componentId);

    /** @brief  Returns component associated with entity
     *
     *  @param  entityId    valid entity id
     *  @param  componentId component id
     *
     *  @return reference to a component
     */
    Component& EntityGetComponent(EntityId entityId, std::size_t componentId) const;

    /** @brief  Checks if there is component associated with entity
     *
     *  @param  entityId    valid entity id
     *  @param  componentId component id
     *
     *  @return @c true if there is component identified by @p componentId, @c false otherwise
     */
    bool EntityHasComponent(EntityId entityId, std::size_t componentId) const;

    /** @brief  Removes a component from entity
     *
     *  @param  entityId    valid entity id
     *  @param  componentId component id
     */
    void EntityDeleteComponent(EntityId entityId, std::size_t componentId);

    /** @brief Removes all components from entity
     *
     *  @param  entityId    valid entity id
     */
    void EntityDeleteComponents(EntityId entityId);

private:
    typedef std::array<ComponentPtr, util::Config::MaxComponentCount> ComponentArray;

    /** @brief  Checks if buffers can fit given number of elements and expands if not
     *
     *  Checks if capacity of @p m_entities is enough to allocate @p count elements
     *  If there is not enough space, buffers are expanded
     *
     *  @param  count   desired number of new elements
     */
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
