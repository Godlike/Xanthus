#ifndef XANTHUS_COMPONENT_PHYSICS_COMPONENT_HPP
#define XANTHUS_COMPONENT_PHYSICS_COMPONENT_HPP

#include "system/physics/PhysicsThread.hpp"

#include "util/Types.hpp"

namespace xanthus
{
namespace component
{

struct PhysicsComponent : public Component
{
    system::physics::BodyHandle* pHandle;
};

}
}

#endif // XANTHUS_COMPONENT_PHYSICS_COMPONENT_HPP
