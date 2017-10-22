#include "system/Intent.hpp"

namespace xanthus
{
namespace system
{

Intent::Intent(WorldTime& worldTime)
    : m_worldTime(worldTime)
{

}

void Intent::Register(Intent::Order order)
{
    m_queue.insert(order);
}

void Intent::Process()
{
    WorldTime::TimeUnit now = m_worldTime.GetTime();

    auto it = m_queue.begin();
    for (; it != m_queue.end();)
    {
        Order order = *it;

        if (order.intent->IsValid())
        {
            if (order.intent->Execute(now))
            {
                it = m_queue.erase(it);
            }
            else
            {
                ++it;
            }
        }
        else
        {
            order.intent->Discard();
            it = m_queue.erase(it);
        }
    }
}

}
}
