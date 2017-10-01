#ifndef XANTHUS_SYSTEM_PHYSICS_BODY_HANDLE_HPP
#define XANTHUS_SYSTEM_PHYSICS_BODY_HANDLE_HPP

#include <atomic>

#include <pegasus/Mechanics.hpp>

namespace xanthus
{
namespace system
{
namespace physics
{

struct BodyHandle
{
    std::atomic<pegasus::RigidBody const*> pBody;
};

}
}
}

#endif // XANTHUS_SYSTEM_PHYSICS_BODY_HANDLE_HPP
