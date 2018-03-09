#ifndef SANDBOX_SYSTEM_PHYSICS_BODY_CONTROLLER_HPP
#define SANDBOX_SYSTEM_PHYSICS_BODY_CONTROLLER_HPP

#include "WorldTime.hpp"

#include "system/physics/BodyHandle.hpp"
#include "system/physics/PegasusAdapter.hpp"
#include "system/physics/SpawnInfo.hpp"

#include <glm/glm.hpp>

#include <list>
#include <mutex>
#include <set>

namespace xanthus
{
namespace system
{
namespace physics
{

class BodyController
{
public:
    BodyController(PegasusAdapter& physicsEngine);

    BodyController(BodyController const& other) = delete;
    BodyController& operator=(BodyController const& other) = delete;

    ~BodyController() = default;

    struct Spawner
    {
        struct Order
        {
            BodyHandle* pHandle;
            WorldTime::TimeUnit spawnTime;

            SpawnInfo info;
        };

        std::mutex mutex;
        std::list<Order> orders;
    };

    struct Pusher
    {
        struct Order
        {
            BodyHandle const* pHandle;
            WorldTime::TimeUnit pushTime;

            glm::vec3 force;
        };

        std::mutex mutex;
        std::list<Order> orders;
    };

    struct Deleter
    {
        struct Order
        {
            BodyHandle const* pHandle;

            WorldTime::TimeUnit deleteTime;
        };

        std::mutex mutex;
        std::list<Order> orders;
    };

    void Spawn(Spawner::Order order);
    void Push(Pusher::Order order);
    void Delete(Deleter::Order order);

    void Check(WorldTime::TimeUnit currentTime);

private:
    void CheckDeleter(WorldTime::TimeUnit currentTime);
    void CheckPusher(WorldTime::TimeUnit currentTime);
    void CheckSpawner(WorldTime::TimeUnit currentTime);

    PegasusAdapter& m_physicsEngine;

    Spawner m_spawner;
    Pusher m_pusher;
    Deleter m_deleter;

    std::set<BodyHandle const*> m_deletedHandles;
};

}
}
}

#endif // SANDBOX_SYSTEM_PHYSICS_BODY_CONTROLLER_HPP
