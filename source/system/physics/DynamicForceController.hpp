#ifndef SANDBOX_SYSTEM_PHYSICS_DYNAMIC_FORCE_CONTROLLER_HPP
#define SANDBOX_SYSTEM_PHYSICS_DYNAMIC_FORCE_CONTROLLER_HPP

#include "WorldTime.hpp"

#include "system/physics/BodyHandle.hpp"
#include "system/physics/PegasusAdapter.hpp"
#include "system/physics/SpawnInfo.hpp"

#include <glm/glm.hpp>

#include <list>
#include <mutex>

namespace xanthus
{
namespace system
{
namespace physics
{

class DynamicForceController
{
public:
    DynamicForceController(PegasusAdapter& physicsEngine);

    DynamicForceController(DynamicForceController const& other) = delete;
    DynamicForceController& operator=(DynamicForceController const& other) = delete;

    ~DynamicForceController() = default;

    struct Creator
    {
        struct Order
        {
            uint32_t id;
            WorldTime::TimeUnit createTime;

            glm::vec3 position;
            double magnitude;
        };

        std::mutex mutex;
        std::list<Order> orders;
    };

    struct Deleter
    {
        struct Order
        {
            uint32_t id;

            WorldTime::TimeUnit deleteTime;
        };

        std::mutex mutex;
        std::list<Order> orders;
    };

    void Create(Creator::Order order);
    void Delete(Deleter::Order order);

    void Check(WorldTime::TimeUnit currentTime);

private:
    void CheckDeleter(WorldTime::TimeUnit currentTime);
    void CheckCreator(WorldTime::TimeUnit currentTime);

    PegasusAdapter& m_physicsEngine;

    Creator m_creator;
    Deleter m_deleter;
};

}
}
}

#endif // SANDBOX_SYSTEM_PHYSICS_DYNAMIC_FORCE_CONTROLLER_HPP
