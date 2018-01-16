#include "system/physics/PhysicsThread.hpp"

#include "util/Config.hpp"
#include "util/ScopeProfiler.hpp"

#include <limits>

namespace xanthus
{
namespace system
{
namespace physics
{

// PhysicsThread

PhysicsThread::PhysicsThread(WorldTime& worldTime)
    : m_threadId(memoryReclaimer.RegisterThread())
    , m_working(true)
    , m_timeControl(worldTime)
    , m_currentPositions(nullptr)
{
    m_currentPositions.store(new BodyPositions());
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
    pHandle->bodyHandle.store(pegasus::scene::Handle());

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

WorldTime::TimeUnit PhysicsThread::GetCurrentTime() const
{
    return WorldTime::TimeUnit(m_timeControl.currentTimeRaw.load());
}

// PhysicsThread::TimeControl

PhysicsThread::TimeControl::TimeControl(WorldTime& worldTime)
    : worldTime(worldTime)
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

        for (TimeControl::TimeUnit future = (m_timeControl.currentTime + util::Config::PhysicsTick); future < target; future += util::Config::PhysicsTick)
        {
            {
                util::ScopeProfiler profiler("pegasus");
                m_physicsEngine.Run(util::Config::PhysicsTick);
            }

            m_timeControl.currentTime = future;

            PollPositions();

            m_timeControl.currentTimeRaw.store(future.count());

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
        newPositions.reserve(m_physicsEngine.primitiveCount + 1);

        newPositions[pegasus::scene::Handle()] = glm::dvec3{
            std::numeric_limits<double>::quiet_NaN()
            , std::numeric_limits<double>::quiet_NaN()
            , std::numeric_limits<double>::quiet_NaN()
        };

        for (pegasus::scene::Primitive const* const primitive : m_physicsEngine.primitives)
        {
            newPositions[primitive->GetBodyHandle()] = primitive->GetBody().linearMotion.position;
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
            pegasus::scene::Handle bodyHandle = order.pHandle->bodyHandle.load();

            m_physicsEngine.DeleteBody(bodyHandle);

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
            pegasus::scene::Handle bodyHandle = m_physicsEngine.SpawnBody(order.info);

            assert(pegasus::scene::Handle() != bodyHandle);

            order.pHandle->bodyHandle.store(bodyHandle);
        }
        else
        {
            break;
        }

        delete it->info.pShape;
        it = m_spawner.orders.erase(it);
    }
}

}
}
}
