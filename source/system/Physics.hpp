#ifndef XANTHUS_SYSTEM_PHYSICS_HPP
#define XANTHUS_SYSTEM_PHYSICS_HPP

#include "entity/World.hpp"

#include "component/Flags.hpp"
#include "component/PositionComponent.hpp"
#include "component/PhysicsComponent.hpp"

#include "system/Skeleton.hpp"

#include "util/Config.hpp"

#include <pegasus/Particle.hpp>
#include <pegasus/ParticleContacts.hpp>
#include <pegasus/ParticleForceGenerator.hpp>
#include <pegasus/ParticleWorld.hpp>

#include <list>
#include <memory>
#include <vector>

namespace xanthus
{
namespace system
{

class Physics : public Skeleton<component::PositionComponent, component::PhysicsComponent>
{
public:
    using TimeUnit = entity::World::TimeUnit;

    enum class Force : std::size_t
    {
        Down    = 0x00
        , Up    = 0x01

        , Count
    };

    Physics(entity::World& world);
    ~Physics() = default;

    void Init();
    void Update(TimeUnit duration);

    pegasus::RigidBody* SpawnBody(pegasus::geometry::SimpleShape* pShape, bool generateContracts, Force force = Force::Down);
    void DeleteBody(pegasus::RigidBody* pBody);

private:
    // Physics part
    using Particles = std::list<pegasus::Particle>;
    using RigidBodies = std::list<pegasus::RigidBody>;
    using ForceGenerators = std::vector<std::unique_ptr<pegasus::ParticleForceGenerator>>;

    using RigidContactMap = std::map<pegasus::RigidBody*, pegasus::ParticleContactGenerator*>;

    Particles m_particles;
    ForceGenerators m_forces;
    RigidBodies m_rigidBodies;

    pegasus::ParticleForceRegistry m_physicsForceRegistry;
    pegasus::ParticleContactGenerators m_physicsContactGenerators;
    pegasus::ParticleWorld m_physicsWorld;

    RigidContactMap m_rigidContactMap;
};

}
}

#endif // XANTHUS_SYSTEM_PHYSICS_HPP
