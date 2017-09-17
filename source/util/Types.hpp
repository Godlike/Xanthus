#ifndef XANTHUS_UTIL_TYPES_HPP
#define XANTHUS_UTIL_TYPES_HPP

#include "util/TemplateCounter.hpp"

#include <cstdint>
#include <limits>
#include <memory>
#include <type_traits>

namespace xanthus
{

typedef uint32_t EntityId;
static EntityId const InvalidEntityId = std::numeric_limits<EntityId>::max();

// Placeholder class for basic RTTI implementation
struct Component
{
    template<class C>
        static inline std::size_t GetId()
    {
        static_assert(true == std::is_base_of<Component, C>::value, "C shall be derived from Component");

        return util::TemplateCounter<Component>::GetId<C>();
    }
};

typedef std::unique_ptr<Component> ComponentPtr;

}

#endif // XANTHUS_UTIL_TYPES_HPP
