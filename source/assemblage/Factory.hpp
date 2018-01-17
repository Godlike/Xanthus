#ifndef XANTHUS_ASSEMBLAGE_FACTORY_HPP
#define XANTHUS_ASSEMBLAGE_FACTORY_HPP

#include "WorldTime.hpp"

#include "assemblage/ProjectileFactory.hpp"

#include "component/GridComponent.hpp"

#include <wink/event_queue.hpp>

#include <glm/glm.hpp>

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

        using Projectile = assemblage::ProjectileFactory::Order;

        wink::event_queue<Projectile> projectiles;
    } orders;

    entity::Entity CreateBox(glm::vec3 position, double size);
    entity::Entity CreateSphere(double radius);
    void ApplySpherePhysics(entity::Entity sphere, double radius, Orders::ParticleEffect impulse);

private:
    struct CustomSpawners
    {
        CustomSpawners(WorldTime& worldTime, Factory& factory, Systems& systems);

        ProjectileFactory projectile;
    };

    void CreateParticleEffect(Orders::ParticleEffect const& order);
    void CreateProjectile(Orders::Projectile const& order);

    WorldTime& m_worldTime;
    entity::World& m_world;
    Systems& m_systems;

    CustomSpawners m_spawners;
};

}
}

#endif // XANTHUS_ASSEMBLAGE_FACTORY_HPP
