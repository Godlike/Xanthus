#include "system/physics/Physics.hpp"

namespace xanthus
{
namespace system
{
namespace physics
{

Physics::Physics(entity::World& world, WorldTime& worldTime)
    : Skeleton<component::PositionComponent, component::PhysicsComponent>(world)
    , m_physicsThread(worldTime, util::Config::PhysicsTick)
    , m_threadId(m_physicsThread.memoryReclaimer.RegisterThread())
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
    entity::World::Entities entities = GetEntities();
    PhysicsThread::BodyPositions const& positions = *m_physicsThread.GetBodyPositions();

    for (entity::Entity& entity : entities)
    {
        component::PositionComponent& posComp = entity.GetComponent<component::PositionComponent>();
        component::PhysicsComponent const& physComp = entity.GetComponent<component::PhysicsComponent>();

        PhysicsThread::BodyPositions::const_iterator cit = positions.find(physComp.pHandle->pBody.load());

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

}
}
}
