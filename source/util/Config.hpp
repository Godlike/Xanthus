#ifndef XANTHUS_UTIL_CONFIG_HPP
#define XANTHUS_UTIL_CONFIG_HPP

#include <cstddef>

#include <chrono>

namespace xanthus
{
namespace util
{
namespace Config
{

static const std::size_t MaxComponentCount = 32;
static const std::size_t MaxPhysicsObjects = 1024;
static const std::size_t InitialEntityCount = 1024;
static const std::chrono::microseconds PhysicsTick = std::chrono::microseconds(1000);

static const float GridSide = 5.0f;
static const float GridAltFactor = 1e-1f;

}
}
}

#endif // XANTHUS_UTIL_CONFIG_HPP
