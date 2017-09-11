#ifndef XANTHUS_COMPONENT_FLAGS_HPP
#define XANTHUS_COMPONENT_FLAGS_HPP

#include "util/Config.hpp"

#include <bitset>

namespace xanthus
{
namespace component
{

class Flags
{
public:
    Flags() = default;

    Flags(Flags const& other) = default;
    Flags& operator=(Flags const& other) = default;

    Flags(Flags&& other) = delete;
    Flags& operator=(Flags&& other) = delete;

    ~Flags() = default;

    bool operator==(Flags const& other) const { return FullMatch(other); }
    bool operator!=(Flags const& other) const { return !PartialMatch(other); }

    bool All() const { return m_flags.all(); }
    bool Any() const { return m_flags.any(); }
    bool None() const { return m_flags.none(); }

    void Clear() { m_flags.reset(); }

    bool FullMatch(Flags const& other) const { return (m_flags & other.m_flags) == m_flags; }
    bool PartialMatch(Flags const& other) const { return (m_flags & other.m_flags).any(); }

    void Set(std::size_t componentId) { m_flags.set(componentId); }
    bool Test(std::size_t componentId) const { return m_flags.test(componentId); }
    void Reset(std::size_t componentId) { m_flags.reset(componentId); }

    template<class Comp = void, class... Comps>
        void Set();

    template<class Comp = void, class... Comps>
        bool Test();

    template<class Comp = void, class... Comps>
        void Reset();

private:
    std::bitset<util::Config::MaxComponentCount> m_flags;
};

}
}

#include "component/Flags.imp"

#endif // XANTHUS_COMPONENT_FLAGS_HPP
