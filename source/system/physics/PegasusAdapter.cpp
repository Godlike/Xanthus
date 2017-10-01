#include "system/physics/PegasusAdapter.hpp"

#include "util/Config.hpp"

namespace xanthus
{
namespace system
{
namespace physics
{

PegasusAdapter::PegasusAdapter()
    : bodyCount(0)
    , m_physicsWorld(m_particles
        , m_physicsForceRegistry
        , m_physicsContactGenerators
        , util::Config::MaxPhysicsObjects * util::Config::MaxPhysicsObjects
        , 5000)
{

}

void PegasusAdapter::Init()
{
    m_rigidContactMap.clear();
    rigidBodies.clear();
    m_forces.clear();
    m_particles.clear();
    m_physicsContactGenerators.clear();
    m_physicsForceRegistry.Clear();

    // Forces
    {
        double const force = 9.8;

        m_forces.resize(static_cast<std::size_t>(Force::Count));

        m_forces[static_cast<std::size_t>(Force::Down)] = std::make_unique<pegasus::ParticleGravity>(glm::dvec3{0, force, 0});
        m_forces[static_cast<std::size_t>(Force::Up)] = std::make_unique<pegasus::ParticleGravity>(glm::dvec3{0, -force, 0});
    }
}

pegasus::RigidBody* PegasusAdapter::SpawnBody(SpawnInfo const& info)
{
    m_particles.emplace_back();
    pegasus::Particle& particle = m_particles.back();

    particle.SetPosition(info.position);
    particle.SetVelocity(info.velocity);

    if (!std::isnan(info.mass))
    {
        particle.SetMass(info.mass);
    }
    else
    {
        particle.SetInverseMass(0.0f);
    }

    particle.SetDamping(info.damping);

    m_physicsForceRegistry.Add(particle, *m_forces[static_cast<std::size_t>(info.force)]);

    rigidBodies.emplace_back(
        particle,
        std::unique_ptr<pegasus::geometry::SimpleShape>(info.pShape)
    );
    ++bodyCount;
    pegasus::RigidBody& rigidBody = rigidBodies.back();

    rigidBody.s->centerOfMass = info.position;

    if (info.generateContacts)
    {
        using RigidContactGenerator = pegasus::ShapeContactGenerator<RigidBodies>;

        RigidContactGenerator* pContact = new RigidContactGenerator(
            rigidBody
            , rigidBodies
            , 0.98f
        );

        m_physicsContactGenerators.push_back(
            std::unique_ptr<RigidContactGenerator>(pContact)
        );

        m_rigidContactMap[&rigidBody] = pContact;
    }

    return &rigidBody;
}

void PegasusAdapter::DeleteBody(pegasus::RigidBody const* pBody)
{
    // Cache particle pointer
    pegasus::Particle* pParticle = &pBody->p;

    // Cleanup rigid body
    {
        auto rigidBodyIt = std::find_if(
            rigidBodies.begin()
            , rigidBodies.end()
            , [=](auto const& it) -> bool
            {
                return &it == pBody;
            }
        );

        if (rigidBodyIt != rigidBodies.end())
        {
            auto rigidContactIt = m_rigidContactMap.find(&*rigidBodyIt);

            if (rigidContactIt != m_rigidContactMap.end())
            {
                auto physicsCGIt = std::find_if(
                    m_physicsContactGenerators.begin()
                    , m_physicsContactGenerators.end()
                    , [=](auto const& it) -> bool
                    {
                        return it.get() == rigidContactIt->second;
                    }
                );

                if (physicsCGIt != m_physicsContactGenerators.end())
                {
                    m_physicsContactGenerators.erase(physicsCGIt);
                }
            }

            rigidBodies.erase(rigidBodyIt);
            --bodyCount;
        }
    }

    // Cleanup particle
    {
        auto particleIt = std::find_if(
            m_particles.begin()
            , m_particles.end()
            , [=](auto const& it) -> bool
            {
                return &it == pParticle;
            }
        );

        if (particleIt != m_particles.end())
        {
            m_physicsForceRegistry.Remove(*particleIt);

            m_particles.erase(particleIt);
        }
    }
}

void PegasusAdapter::Run(WorldTime::TimeUnit tick)
{
    m_physicsWorld.StartFrame();

    m_physicsWorld.RunPhysics(
        static_cast<float>(tick.count())
        * (static_cast<float>(decltype(tick)::period::num)
            / static_cast<float>(decltype(tick)::period::den)
        )
    );

    for (pegasus::RigidBody const& body : rigidBodies)
    {
        body.s->centerOfMass = body.p.GetPosition();
    }
}

}
}
}
