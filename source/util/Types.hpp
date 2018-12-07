#ifndef XANTHUS_UTIL_TYPES_HPP
#define XANTHUS_UTIL_TYPES_HPP

#include "util/TemplateCounter.hpp"

#include <cstdint>
#include <limits>
#include <memory>
#include <type_traits>

namespace xanthus
{

enum class Force : std::size_t
{
    Down    = 0x00
    , Up    = 0x01

    , Count
};

}

#endif // XANTHUS_UTIL_TYPES_HPP
