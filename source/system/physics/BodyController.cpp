#include "system/physics/BodyController.hpp"

#include <pegasus/Scene.hpp>

namespace xanthus
{
namespace system
{
namespace physics
{

BodyController::BodyController(PegasusAdapter& physicsEngine)
    : m_physicsEngine(physicsEngine)
{

}

void BodyController::Spawn(Spawner::Order order)
{
    std::lock_guard<std::mutex> lock(m_spawner.mutex);

    m_spawner.orders.push_back(order);
}

void BodyController::Push(Pusher::Order order)
{
    std::lock_guard<std::mutex> lock(m_pusher.mutex);

    m_pusher.orders.push_back(order);
}

void BodyController::Delete(Deleter::Order order)
{
    std::lock_guard<std::mutex> lock(m_deleter.mutex);

    m_deleter.orders.push_back(order);
}

void BodyController::Check(WorldTime::TimeUnit currentTime)
{
    CheckDeleter(currentTime);
    CheckSpawner(currentTime);
    CheckPusher(currentTime);

    m_deletedHandles.clear();
}

void BodyController::CheckDeleter(WorldTime::TimeUnit currentTime)
{
    std::lock_guard<std::mutex> lock(m_deleter.mutex);

    std::list<Deleter::Order>::const_iterator it = m_deleter.orders.begin();
    while (it != m_deleter.orders.end())
    {
        Deleter::Order const& order = *it;

        if (order.deleteTime <= currentTime)
        {
            m_physicsEngine.DeleteBody(order.pHandle->bodyHandle.load());

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

void BodyController::CheckPusher(WorldTime::TimeUnit currentTime)
{
    std::lock_guard<std::mutex> lock(m_pusher.mutex);

    std::list<Pusher::Order>::const_iterator it = m_pusher.orders.begin();

    // Remove deleted handles
    while (it != m_pusher.orders.end())
    {
        if (0 != m_deletedHandles.count(it->pHandle))
        {
            it = m_pusher.orders.erase(it);
        }
        else
        {
            ++it;
        }
    }

    it = m_pusher.orders.begin();

    // Execute spawner orders
    while (it != m_pusher.orders.end())
    {
        Pusher::Order const& order = *it;

        if (order.pushTime <= currentTime)
        {
            m_physicsEngine.PushBody(order.pHandle->bodyHandle.load(), order.force);

            it = m_pusher.orders.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void BodyController::CheckSpawner(WorldTime::TimeUnit currentTime)
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

        if (order.spawnTime <= currentTime)
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
