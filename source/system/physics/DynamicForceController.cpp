#include "system/physics/DynamicForceController.hpp"

namespace xanthus
{
namespace system
{
namespace physics
{

DynamicForceController::DynamicForceController(PegasusAdapter& physicsEngine)
    : m_physicsEngine(physicsEngine)
{

}

void DynamicForceController::Create(Creator::Order order)
{
    std::lock_guard<std::mutex> lock(m_creator.mutex);

    m_creator.orders.push_back(order);
}

void DynamicForceController::Delete(Deleter::Order order)
{
    std::lock_guard<std::mutex> lock(m_deleter.mutex);

    m_deleter.orders.push_back(order);
}

void DynamicForceController::Check(WorldTime::TimeUnit currentTime)
{
    CheckDeleter(currentTime);
    CheckCreator(currentTime);
}

void DynamicForceController::CheckDeleter(WorldTime::TimeUnit currentTime)
{
    std::lock_guard<std::mutex> lock(m_deleter.mutex);

    std::list<Deleter::Order>::const_iterator it = m_deleter.orders.begin();

    while (it != m_deleter.orders.end())
    {
        Deleter::Order const& order = *it;

        if (order.deleteTime <= currentTime)
        {
            m_physicsEngine.DeleteGravitySource(order.id);

            it = m_deleter.orders.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void DynamicForceController::CheckCreator(WorldTime::TimeUnit currentTime)
{
    std::lock_guard<std::mutex> lock(m_creator.mutex);

    std::list<Creator::Order>::const_iterator it = m_creator.orders.begin();

    while (it != m_creator.orders.end())
    {
        Creator::Order const& order = *it;

        if (order.createTime <= currentTime)
        {
            m_physicsEngine.CreateGravitySource(order.id, order.position, order.magnitude);
        }
        else
        {
            break;
        }

        it = m_creator.orders.erase(it);
    }
}

}
}
}
