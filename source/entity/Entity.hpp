#ifndef XANTHUS_ENTITY_ENTITY_HPP
#define XANTHUS_ENTITY_ENTITY_HPP

#include "util/Types.hpp"

namespace xanthus
{
namespace entity
{

class World;

class Entity
{
public:
    Entity();
    Entity(World* world, EntityId id);

    Entity(const Entity& other) = default;
    Entity& operator=(const Entity& other) = default;

    ~Entity() = default;

    bool operator==(const Entity& other) const;
    bool operator!=(const Entity& other) const { return !operator==(other); }

    EntityId GetId() const { return m_id; }
    bool IsValid() const;

    void Destroy();

    template<class C, typename... Args>
        C& AddComponent(Args&&... args);

    template<class C>
        bool HasComponent() const;

    template<class C>
        C& GetComponent() const;

    template<class C>
        void DeleteComponent();

private:
    void AddComponentImpl(Component* pComponent, std::size_t componentId);
    Component& GetComponentImpl(std::size_t componentId) const;
    bool HasComponentImpl(std::size_t componentId) const;
    void DeleteComponentImpl(std::size_t componentId);

    EntityId m_id;
    World* m_pWorld;
};

}
}

#include "entity/Entity.imp"

#endif // XANTHUS_ENTITY_ENTITY_HPP
