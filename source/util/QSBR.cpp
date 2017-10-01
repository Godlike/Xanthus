#include "util/QSBR.hpp"

#include <cassert>

namespace xanthus
{
namespace util
{

QSBR::QSBR() : m_remainingThreads(0)
{

}

QSBR::~QSBR()
{
    for (auto const& callback : m_previousInterval)
    {
        callback();
    }

    for (auto const& callback : m_currentInterval)
    {
        callback();
    }
}

QSBR::ThreadIndex QSBR::RegisterThread()
{
    QSBR::ThreadIndex id;

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        id = m_quiescentThreads.size();
        m_quiescentThreads.push_back(false);
        ++m_remainingThreads;
    }

    return id;
}

void QSBR::OnQuiescentState(ThreadIndex id)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    assert(id < m_quiescentThreads.size());

    if (!m_quiescentThreads[id])
    {
        m_quiescentThreads[id] = true;

        if (0 == --m_remainingThreads)
        {
            for (auto const& callback : m_previousInterval)
            {
                callback();
            }

            m_previousInterval = std::move(m_currentInterval);
            m_currentInterval.clear();

            for (std::size_t i = 0; i < m_quiescentThreads.size(); ++i)
            {
                m_quiescentThreads[i] = false;
            }

            m_remainingThreads = m_quiescentThreads.size();
        }
    }
}

void QSBR::AddCallback(Callback const& callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_currentInterval.push_back(callback);
}

}
}
