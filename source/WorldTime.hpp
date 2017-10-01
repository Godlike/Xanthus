#ifndef XANTHUS_WORLD_TIME_HPP
#define XANTHUS_WORLD_TIME_HPP

#include <atomic>
#include <chrono>
#include <cstdint>

namespace xanthus
{

class WorldTime
{
public:
    using TimeUnit = std::chrono::microseconds;

    WorldTime() : m_time(0) {}
    ~WorldTime() = default;

    void SetTime(TimeUnit time) { m_time.store(time.count()); }
    TimeUnit GetTime() const { return TimeUnit(m_time.load()); }

private:
    std::atomic<uint64_t> m_time;
};

}

#endif // XANTHUS_WORLD_TIME_HPP
