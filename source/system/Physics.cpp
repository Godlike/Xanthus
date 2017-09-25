#include "system/Physics.hpp"

namespace xanthus
{
namespace system
{

Physics::Physics(entity::World& world)
    : Skeleton<component::PositionComponent, component::PhysicsComponent>(world)
    , m_physicsWorld(m_particles
        , m_physicsForceRegistry
        , m_physicsContactGenerators
        , util::Config::MaxPhysicsObjects * util::Config::MaxPhysicsObjects
        , 5000)
{

}

void Physics::Init()
{
    m_rigidContactMap.clear();
    m_rigidBodies.clear();
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

void Physics::Update(TimeUnit duration)
{
    m_physicsWorld.StartFrame();

    m_physicsWorld.RunPhysics(static_cast<float>(duration.count()) / 1000.f);

    entity::World::Entities entities = GetEntities();

    for (entity::Entity& entity : entities)
    {
        component::PositionComponent& posComp = entity.GetComponent<component::PositionComponent>();
        component::PhysicsComponent const& physComp = entity.GetComponent<component::PhysicsComponent>();

        posComp.position = static_cast<glm::vec3>(physComp.pBody->p.GetPosition());
        physComp.pBody->s->centerOfMass = posComp.position;
    }
}

pegasus::RigidBody* Physics::SpawnBody(pegasus::geometry::SimpleShape* pShape, bool generateContacts, Force force)
{
    m_particles.emplace_back();
    pegasus::Particle& particle = m_particles.back();

    m_physicsForceRegistry.Add(particle, *m_forces[static_cast<std::size_t>(force)]);

    m_rigidBodies.emplace_back(
        particle,
        std::unique_ptr<pegasus::geometry::SimpleShape>(pShape)
    );
    pegasus::RigidBody& rigidBody = m_rigidBodies.back();

    if (generateContacts)
    {
        using RigidContactGenerator = pegasus::ShapeContactGenerator<RigidBodies>;

        RigidContactGenerator* pContact = new RigidContactGenerator(
            rigidBody
            , m_rigidBodies
            , 0.5f
        );

        m_physicsContactGenerators.push_back(
            std::unique_ptr<RigidContactGenerator>(pContact)
        );

        m_rigidContactMap[&rigidBody] = pContact;
    }

    return &rigidBody;
}

void Physics::DeleteBody(pegasus::RigidBody* pBody)
{
    // Cache particle pointer
    pegasus::Particle* pParticle = &pBody->p;

    // Cleanup rigid body
    {
        auto rigidBodyIt = std::find_if(
            m_rigidBodies.begin()
            , m_rigidBodies.end()
            , [=](auto const& it) -> bool
            {
                return &it == pBody;
            }
        );

        if (rigidBodyIt != m_rigidBodies.end())
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

            m_rigidBodies.erase(rigidBodyIt);
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

}
}
