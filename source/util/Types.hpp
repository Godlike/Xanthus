#ifndef XANTHUS_UTIL_TYPES_HPP
#define XANTHUS_UTIL_TYPES_HPP

#include <cstdint>
#include <limits>
#include <memory>

namespace xanthus
{

typedef uint32_t EntityId;
static EntityId const InvalidEntityId = std::numeric_limits<EntityId>::max();

// Placeholder class for basic RTTI implementation
struct Component {};
typedef std::unique_ptr<Component> ComponentPtr;

}

#endif // XANTHUS_UTIL_TYPES_HPP
