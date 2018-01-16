#ifndef XANTHUS_SYSTEM_PHYSICS_PHYSICS_THREAD_HPP
#define XANTHUS_SYSTEM_PHYSICS_PHYSICS_THREAD_HPP

#include "WorldTime.hpp"

#include "system/physics/BodyHandle.hpp"
#include "system/physics/PegasusAdapter.hpp"
#include "system/physics/SpawnInfo.hpp"

#include "util/QSBR.hpp"

#include <atomic>
#include <mutex>
#include <set>
#include <thread>
#include <unordered_map>

namespace xanthus
{
namespace system
{
namespace physics
{

class PhysicsThread
{
public:
    using BodyPositions = std::unordered_map<pegasus::scene::Handle, glm::dvec3>;
    using ThreadIndex = util::QSBR::ThreadIndex;

    PhysicsThread(WorldTime& worldTime);
    ~PhysicsThread();

    void Init();
    void Run();
    void Join();

    BodyPositions* GetBodyPositions() const;

    BodyHandle* SpawnBody(SpawnInfo const& info);
    void DeleteBody(BodyHandle const* pHandle);

    WorldTime::TimeUnit GetCurrentTime() const;

    util::QSBR memoryReclaimer;

private:
    struct TimeControl
    {
        using TimeUnit = WorldTime::TimeUnit;

        TimeControl(WorldTime& worldTime);

        WorldTime& worldTime;
        TimeUnit currentTime;

        std::atomic<uint64_t> currentTimeRaw;
    };

    struct Spawner
    {
        struct Order
        {
            BodyHandle* pHandle;
            TimeControl::TimeUnit spawnTime;

            SpawnInfo info;
        };

        std::mutex mutex;
        std::list<Order> orders;
    };

    struct Deleter
    {
        struct Order
        {
            BodyHandle const* pHandle;

            TimeControl::TimeUnit deleteTime;
        };

        std::mutex mutex;
        std::list<Order> orders;
    };

    void Routine();

    void PollPositions();
    void CheckDeleter();
    void CheckSpawner();

    ThreadIndex m_threadId;
    std::thread m_thread;
    std::atomic_bool m_working;

    TimeControl m_timeControl;

    std::atomic<BodyPositions*> m_currentPositions;

    PegasusAdapter m_physicsEngine;

    Spawner m_spawner;
    Deleter m_deleter;

    std::set<BodyHandle const*> m_deletedHandles;
};

}
}
}

#endif // XANTHUS_SYSTEM_PHYSICS_PHYSICS_THREAD_HPP
