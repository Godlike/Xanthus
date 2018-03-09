#include "system/physics/Physics.hpp"

namespace xanthus
{
namespace system
{
namespace physics
{

// Physics::

Physics::Physics(entity::World& world, WorldTime& worldTime)
    : Skeleton<component::PositionComponent, component::PhysicsComponent>(world)
    , m_physicsThread(worldTime)
    , m_threadId(m_physicsThread.memoryReclaimer.RegisterThread())
    , m_control(world, m_physicsThread)
{

}

Physics::~Physics()
{
    m_physicsThread.Join();
}

void Physics::Init()
{
    m_physicsThread.Init();
    m_physicsThread.Run();
}

void Physics::Update()
{
    m_control.Update();

    entity::World::Entities entities = GetEntities();
    PhysicsThread::BodyPositions const& positions = *m_physicsThread.GetBodyPositions();

    for (entity::Entity& entity : entities)
    {
        component::PositionComponent& posComp = entity.GetComponent<component::PositionComponent>();
        component::PhysicsComponent const& physComp = entity.GetComponent<component::PhysicsComponent>();

        PhysicsThread::BodyPositions::const_iterator cit = positions.find(physComp.pHandle->bodyHandle.load());

        if (positions.cend() != cit)
        {
            posComp.position = static_cast<glm::vec3>(cit->second);
        }
    }

    m_physicsThread.memoryReclaimer.OnQuiescentState(m_threadId);
}

BodyHandle* Physics::SpawnBody(SpawnInfo const& info)
{
    return m_physicsThread.SpawnBody(info);
}

void Physics::DeleteBody(BodyHandle const* pHandle)
{
    m_physicsThread.DeleteBody(pHandle);
}

void Physics::CreateGravitySource(uint32_t id, glm::vec3 position, double magnitude)
{
    m_physicsThread.CreateGravitySource(id, position, magnitude);
}

void Physics::DeleteGravitySource(uint32_t id)
{
    m_physicsThread.DeleteGravitySource(id);
}

// Physics::Control::

Physics::Control::Control(entity::World& world, PhysicsThread& physicsThread)
    : Skeleton<component::PhysicsComponent, component::ControlComponent>(world)
    , m_physicsThread(physicsThread)
{

}

void Physics::Control::Update()
{
    entity::World::Entities entities = GetEntities();

    for (entity::Entity& entity : entities)
    {
        component::PhysicsComponent const& physComp = entity.GetComponent<component::PhysicsComponent>();
        component::ControlComponent const& controlComp = entity.GetComponent<component::ControlComponent>();

        m_physicsThread.PushBody(physComp.pHandle, controlComp.force);

        entity.DeleteComponent<component::ControlComponent>();
    }
}

}
}
}
