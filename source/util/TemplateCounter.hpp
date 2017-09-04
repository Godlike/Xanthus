#ifndef XANTHUS_UTIL_TEMPLATE_COUNTER_HPP
#define XANTHUS_UTIL_TEMPLATE_COUNTER_HPP

#include <cstddef>

namespace xanthus
{
namespace util
{

template<typename CounterName>
    class TemplateCounter
{
public:
    template<typename T>
        static std::size_t GetId()
        {
            static const std::size_t id = m_counter++;
            return id;
        }

private:
    static std::size_t m_counter;
};

template<typename CounterName>
    std::size_t TemplateCounter<CounterName>::m_counter = 0;

}
}

#endif // XANTHUS_UTIL_TEMPLATE_COUNTER_HPP
