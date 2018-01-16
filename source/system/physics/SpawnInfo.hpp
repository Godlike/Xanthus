#ifndef XANTHUS_SYSTEM_PHYSICS_SPAWN_INFO_HPP
#define XANTHUS_SYSTEM_PHYSICS_SPAWN_INFO_HPP

#include "WorldTime.hpp"

#include "util/Types.hpp"

#include <Arion/Shape.hpp>

namespace xanthus
{
namespace system
{
namespace physics
{

struct SpawnInfo
{
    arion::SimpleShape* pShape;
    glm::dvec3 position;
    glm::dvec3 velocity;

    double mass;
    double damping;

    Force force;
};

}
}
}

#endif // XANTHUS_SYSTEM_PHYSICS_SPAWN_INFO_HPP
