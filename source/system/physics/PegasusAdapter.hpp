#ifndef XANTHUS_SYSTEM_PHYSICS_PEGASUS_ADAPTER_HPP
#define XANTHUS_SYSTEM_PHYSICS_PEGASUS_ADAPTER_HPP

#include "WorldTime.hpp"

#include "system/physics/SpawnInfo.hpp"

#include <pegasus/Mechanics.hpp>
#include <pegasus/Particle.hpp>
#include <pegasus/ParticleContacts.hpp>
#include <pegasus/ParticleForceGenerator.hpp>
#include <pegasus/ParticleWorld.hpp>

#include <cstddef>
#include <list>
#include <map>
#include <vector>

namespace xanthus
{
namespace system
{
namespace physics
{

class PegasusAdapter
{
public:
    using RigidBodies = std::list<pegasus::RigidBody>;

    PegasusAdapter();
    ~PegasusAdapter() = default;

    void Init();

    pegasus::RigidBody* SpawnBody(SpawnInfo const& info);
    void DeleteBody(pegasus::RigidBody const* pBody);

    void Run(WorldTime::TimeUnit tick);

    RigidBodies rigidBodies;
    std::size_t bodyCount;

private:
    using Particles = pegasus::Particles;
    using ForceGenerators = std::vector<std::unique_ptr<pegasus::ParticleForceGenerator>>;

    using RigidContactMap = std::map<pegasus::RigidBody*, pegasus::ParticleContactGenerator*>;

    Particles m_particles;
    ForceGenerators m_forces;

    pegasus::ParticleForceRegistry m_physicsForceRegistry;
    pegasus::ParticleContactGenerators m_physicsContactGenerators;
    pegasus::ParticleWorld m_physicsWorld;

    RigidContactMap m_rigidContactMap;
};

}
}
}

#endif // XANTHUS_SYSTEM_PHYSICS_PEGASUS_ADAPTER_HPP
