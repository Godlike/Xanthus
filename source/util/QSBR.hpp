#ifndef XANTHUS_UTIL_QSBR_HPP
#define XANTHUS_UTIL_QSBR_HPP

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>

namespace xanthus
{
namespace util
{

class QSBR
{
public:
    using ThreadIndex = std::size_t;
    using Callback = std::function<void()>;

    QSBR();

    QSBR(const QSBR& other) = delete;
    QSBR& operator=(const QSBR& other) = delete;

    ~QSBR();

    ThreadIndex RegisterThread();
    void OnQuiescentState(ThreadIndex id);
    void AddCallback(Callback const& callback);

private:
    std::mutex m_mutex;
    std::vector<bool> m_quiescentThreads;
    std::vector<Callback> m_currentInterval;
    std::vector<Callback> m_previousInterval;

    uint32_t m_remainingThreads;
};

}
}

#endif // XANTHUS_UTIL_QSBR_HPP
