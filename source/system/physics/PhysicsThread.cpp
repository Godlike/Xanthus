#include "system/physics/PhysicsThread.hpp"

#include <limits>

namespace xanthus
{
namespace system
{
namespace physics
{

// PhysicsThread

PhysicsThread::PhysicsThread(WorldTime& worldTime
    , WorldTime::TimeUnit tick
)
    : m_threadId(memoryReclaimer.RegisterThread())
    , m_working(true)
    , m_timeControl(worldTime, tick)
    , m_currentPositions(nullptr)
{

}

PhysicsThread::~PhysicsThread()
{
    delete m_currentPositions.load();
}

void PhysicsThread::Init()
{
    m_physicsEngine.Init();
}

void PhysicsThread::Run()
{
    m_thread = std::thread(std::bind(&PhysicsThread::Routine, this));
}

void PhysicsThread::Join()
{
    m_working.store(false);

    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

PhysicsThread::BodyPositions* PhysicsThread::GetBodyPositions() const
{
    return m_currentPositions.load();
}

BodyHandle* PhysicsThread::SpawnBody(SpawnInfo const& info)
{
    BodyHandle* pHandle = new BodyHandle();
    pHandle->pBody.store(nullptr);

    {
        std::lock_guard<std::mutex> lock(m_spawner.mutex);

        m_spawner.orders.push_back({
            pHandle
            , m_timeControl.worldTime.GetTime()
            , info
        });
    }

    return pHandle;
}

void PhysicsThread::DeleteBody(BodyHandle const* pHandle)
{
    std::lock_guard<std::mutex> lock(m_deleter.mutex);

    m_deleter.orders.push_back({
        pHandle
        , m_timeControl.worldTime.GetTime()
    });
}

// PhysicsThread::TimeControl

PhysicsThread::TimeControl::TimeControl(WorldTime& worldTime
    , TimeUnit tick
)
    : worldTime(worldTime)
    , tick(tick)
    , currentTime(worldTime.GetTime())
{

}

// PhysicsThread

void PhysicsThread::Routine()
{
    TimeControl::TimeUnit target;

    do
    {
        target = m_timeControl.worldTime.GetTime();

        for (TimeControl::TimeUnit future = (m_timeControl.currentTime + m_timeControl.tick); future < target; future += m_timeControl.tick)
        {
            m_physicsEngine.Run(m_timeControl.tick);

            m_timeControl.currentTime = future;

            PollPositions();

            memoryReclaimer.OnQuiescentState(m_threadId);

            CheckDeleter();
            CheckSpawner();
            m_deletedHandles.clear();
        }

        std::this_thread::yield();
    } while (m_working.load());
}

void PhysicsThread::PollPositions()
{
    BodyPositions* pOldPositions = m_currentPositions.load();
    BodyPositions* pNewPositions = new BodyPositions();

    {
        BodyPositions& newPositions = *pNewPositions;
        newPositions.reserve(m_physicsEngine.bodyCount + 1);

        newPositions[nullptr] = glm::dvec3{
            std::numeric_limits<double>::quiet_NaN()
            , std::numeric_limits<double>::quiet_NaN()
            , std::numeric_limits<double>::quiet_NaN()
        };

        for (pegasus::RigidBody const& body : m_physicsEngine.rigidBodies)
        {
            newPositions[&body] = body.p.GetPosition();
        }
    }

    m_currentPositions.store(pNewPositions);

    memoryReclaimer.AddCallback([=](){ delete pOldPositions; });
}

void PhysicsThread::CheckDeleter()
{
    std::lock_guard<std::mutex> lock(m_deleter.mutex);

    std::list<Deleter::Order>::const_iterator it = m_deleter.orders.begin();
    while (it != m_deleter.orders.end())
    {
        Deleter::Order const& order = *it;

        if (order.deleteTime <= m_timeControl.currentTime)
        {
            pegasus::RigidBody const* pBody = order.pHandle->pBody.load();

            if (pBody)
            {
                m_physicsEngine.DeleteBody(pBody);
            }

            m_deletedHandles.insert(order.pHandle);
            delete order.pHandle;

            it = m_deleter.orders.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void PhysicsThread::CheckSpawner()
{
    std::lock_guard<std::mutex> lock(m_spawner.mutex);

    std::list<Spawner::Order>::const_iterator it = m_spawner.orders.begin();

    // Remove deleted handles
    while (it != m_spawner.orders.end())
    {
        if (0 != m_deletedHandles.count(it->pHandle))
        {
            it = m_spawner.orders.erase(it);
        }
        else
        {
            ++it;
        }
    }

    it = m_spawner.orders.begin();

    // Execute spawner orders
    while (it != m_spawner.orders.end())
    {
        Spawner::Order const& order = *it;

        if (order.spawnTime <= m_timeControl.currentTime)
        {
            order.pHandle->pBody.store(m_physicsEngine.SpawnBody(order.info));
        }
        else
        {
            break;
        }

        it = m_spawner.orders.erase(it);
    }
}

}
}
}
