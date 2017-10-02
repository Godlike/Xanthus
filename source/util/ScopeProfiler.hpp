#ifndef XANTHUS_UTIL_SCOPE_PROFILER_HPP
#define XANTHUS_UTIL_SCOPE_PROFILER_HPP

#include <chrono>
#include <iostream>

namespace xanthus
{
namespace util
{

class ScopeProfiler
{
public:
    ScopeProfiler(const char* name)
    : m_name(name)
    {
        m_start = Clock::now();
    }

    ~ScopeProfiler()
    {
        std::cerr << "[Profiler] #" << m_name << " "
            << std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - m_start).count() << "us"
            << std::endl;
    }

private:
    using Clock = std::chrono::high_resolution_clock;

    char const* m_name;
    Clock::time_point m_start;
};

}
}

#endif // XANTHUS_UTIL_SCOPE_PROFILER_HPP