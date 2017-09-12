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

    /** @brief  Checks if flag sets are the same */
    bool operator==(Flags const& other) const { return m_flags == other.m_flags; }

    /** @brief  Checks if flags sets are different */
    bool operator!=(Flags const& other) const { return !operator==(other); }

    /** @brief  Checks if all flags are set */
    bool All() const { return m_flags.all(); }

    /** @brief  Checks if any flag is set */
    bool Any() const { return m_flags.any(); }

    /** @brief  Checks if no flags are set */
    bool None() const { return m_flags.none(); }

    /** @brief  Resets all flags */
    void Clear() { m_flags.reset(); }

    /** @brief  Checks if all flags from @p this are set in @p other
     *
     *  @param  other   rhs
     *
     *  @return @c true if all flags in @p m_flags are set in @p other, @c false otherwise
     */
    bool FullMatch(Flags const& other) const { return (m_flags & other.m_flags) == m_flags; }

    /** @brief  Checks if there is intersection of flags with @p other
     *
     *  @param  other   rhs
     *
     *  @return @c true if @p m_flags are empty or there is intersection in set flags with
     *          @p other, @c false otherwise
     */
    bool PartialMatch(Flags const& other) const
    {
        return m_flags.none() || (m_flags & other.m_flags).any();
    }

    /** @brief  Sets flag for given ID
     *
     *  @param  componentId flag ID to set
     */
    void Set(std::size_t componentId) { m_flags.set(componentId); }

    /** @brief  Checks if flag is set for given ID */
    bool Test(std::size_t componentId) const { return m_flags.test(componentId); }

    /** @brief  Resets flag for given ID
     *
     *  @param  componentId flag ID to reset
     */
    void Reset(std::size_t componentId) { m_flags.reset(componentId); }

    /** @brief  Sets flags for given components
     *
     *  @tparam Comp    component to gather ID from
     *  @tparam Comps   additional components
     */
    template<class Comp = void, class... Comps>
        void Set();

    /** @brief  Checks if all flags are set for given components
     *
     *  @tparam Comp    component
     *  @tparam Comps   additional components
     *
     *  @return @c true if all flags for given components are set, @c false otherwise
     */
    template<class Comp = void, class... Comps>
        bool Test();

    /** @brief  Resets flags for given components
     *
     *  @tparam Comp    component
     *  @tparam Comps   additional components
     */
    template<class Comp = void, class... Comps>
        void Reset();

private:
    //! Holds flags for components
    std::bitset<util::Config::MaxComponentCount> m_flags;
};

}
}

#include "component/Flags.imp"

#endif // XANTHUS_COMPONENT_FLAGS_HPP
