#ifndef XANTHUS_ASSEMBLAGE_FACTORY_HPP
#define XANTHUS_ASSEMBLAGE_FACTORY_HPP

#include <sleipnir/ecs/WorldTime.hpp>

#include "assemblage/ProjectileFactory.hpp"

#include "component/GridComponent.hpp"

#include <Arion/Shape.hpp>

#include <sleipnir/ecs/system/physics/Physics.hpp>
#include <sleipnir/ecs/system/physics/Primitives.hpp>
#include <sleipnir/ecs/system/Render.hpp>
#include <sleipnir/SleipnirEngine.hpp>

#include <wink/event_queue.hpp>

#include <glm/glm.hpp>

#include <random>

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
    using WorldTime = sleipnir::ecs::WorldTime;

    Factory(sleipnir::SleipnirEngine& engine);
    ~Factory() = default;

    void ExecuteOrders();
    void ReclaimEntity(sleipnir::ecs::entity::Entity const& entity);

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

    sleipnir::ecs::entity::Entity CreateBox(arion::Box const& box
        , std::uniform_real_distribution<> colorDistribution = std::uniform_real_distribution<>(0.6, 0.8)
        , double mass = std::numeric_limits<double>::quiet_NaN(), bool physics = true);

    sleipnir::ecs::entity::Entity CreateSphere(arion::Sphere const& sphere);
    sleipnir::ecs::entity::Entity CreatePlane(arion::Plane const& plane);

    void ApplySpherePhysics(sleipnir::ecs::entity::Entity sphere, double radius, Orders::ParticleEffect impulse);
    void ApplyGravitySource(sleipnir::ecs::entity::Entity sphere, double radius, double magnitude);

private:
    struct CustomSpawners
    {
        CustomSpawners(sleipnir::SleipnirEngine& engine, Factory& factory);

        ProjectileFactory projectile;
    };

    void CreateParticleEffect(Orders::ParticleEffect const& order);
    void CreateProjectile(Orders::Projectile const& order);

    WorldTime& m_worldTime;
    sleipnir::ecs::entity::World& m_world;

    sleipnir::ecs::system::Render& m_render;
    sleipnir::ecs::system::physics::BodyChanges::Instance m_physicsBodyChanges;

    CustomSpawners m_spawners;
};

}
}

#endif // XANTHUS_ASSEMBLAGE_FACTORY_HPP
