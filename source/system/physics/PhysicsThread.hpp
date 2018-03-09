#ifndef XANTHUS_SYSTEM_PHYSICS_PHYSICS_THREAD_HPP
#define XANTHUS_SYSTEM_PHYSICS_PHYSICS_THREAD_HPP

#include "WorldTime.hpp"

#include "system/physics/BodyController.hpp"
#include "system/physics/BodyHandle.hpp"
#include "system/physics/DynamicForceController.hpp"
#include "system/physics/PegasusAdapter.hpp"
#include "system/physics/SpawnInfo.hpp"

#include "util/QSBR.hpp"

#include <atomic>
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
    void PushBody(BodyHandle const* pHandle, glm::vec3 force);
    void DeleteBody(BodyHandle const* pHandle);

    void CreateGravitySource(uint32_t id, glm::vec3 position, double magnitude);
    void DeleteGravitySource(uint32_t id);

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

    void Routine();

    void PollPositions();

    ThreadIndex m_threadId;
    std::thread m_thread;
    std::atomic_bool m_working;

    TimeControl m_timeControl;

    std::atomic<BodyPositions*> m_currentPositions;

    PegasusAdapter m_physicsEngine;

    DynamicForceController m_dynamicForceController;
    BodyController m_bodyController;
};

}
}
}

#endif // XANTHUS_SYSTEM_PHYSICS_PHYSICS_THREAD_HPP
