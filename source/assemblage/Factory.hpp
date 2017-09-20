#ifndef XANTHUS_ASSEMBLAGE_FACTORY_HPP
#define XANTHUS_ASSEMBLAGE_FACTORY_HPP

#include "Systems.hpp"

#include "component/PositionComponent.hpp"

#include <wink/event_queue.hpp>

namespace xanthus
{

namespace entity
{
    class Entity;
    class World;
}

namespace assemblage
{

class Factory
{
public:
    Factory(entity::World& world, Systems& systems);
    ~Factory() = default;

    void ExecuteOrders();
    void ReclaimEntity(entity::Entity const& entity);

    struct Orders
    {
        struct ParticleEffect
        {
            glm::vec3 position;
            entity::World::TimeUnit ttl;
            std::size_t count;
        };

        wink::event_queue<ParticleEffect> particleEffects;

        struct Plane
        {
            glm::vec3 position;
            glm::vec3 normal;
        };

        wink::event_queue<Plane> planes;
    } orders;
private:
    void CreateParticleEffect(Orders::ParticleEffect const& order);
    void CreatePlane(Orders::Plane const& order);

    entity::World& m_world;
    Systems& m_systems;
};

}
}

#endif // XANTHUS_ASSEMBLAGE_FACTORY_HPP
