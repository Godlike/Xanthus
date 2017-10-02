#ifndef XANTHUS_ASSEMBLAGE_FACTORY_HPP
#define XANTHUS_ASSEMBLAGE_FACTORY_HPP

#include "WorldTime.hpp"

#include "component/ValueAnimationComponent.hpp"

#include <wink/event_queue.hpp>

namespace xanthus
{

class Systems;

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
    Factory(WorldTime& worldTime, entity::World& world, Systems& systems);
    ~Factory() = default;

    void ExecuteOrders();
    void ReclaimEntity(entity::Entity const& entity);

    struct Orders
    {
        struct Dummy
        {
            glm::vec3 position;
        };

        wink::event_queue<Dummy> dummies;

        struct ParticleEffect
        {
            enum class Type : uint8_t
            {
                Down
                , Up
            };

            glm::vec3 position;
            glm::vec3 velocity;

            WorldTime::TimeUnit ttl;

            std::size_t count;
            Type type;
        };

        wink::event_queue<ParticleEffect> particleEffects;

        struct Plane
        {
            glm::vec3 position;
            glm::vec3 normal;
        };

        wink::event_queue<Plane> planes;

        struct Projectile
        {
            glm::vec3 position;

            component::ValueAnimationComponent animationInfo;
        };

        wink::event_queue<Projectile> projectiles;
    } orders;
private:
    void CreateDummy(Orders::Dummy const& order);
    void CreateParticleEffect(Orders::ParticleEffect const& order);
    void CreatePlane(Orders::Plane const& order);
    void CreateProjectile(Orders::Projectile const& order);

    WorldTime& m_worldTime;
    entity::World& m_world;
    Systems& m_systems;
};

}
}

#endif // XANTHUS_ASSEMBLAGE_FACTORY_HPP
