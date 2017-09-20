#ifndef XANTHUS_COMPONENT_PHYSICS_COMPONENT_HPP
#define XANTHUS_COMPONENT_PHYSICS_COMPONENT_HPP

#include "util/Types.hpp"

#include <pegasus/Particle.hpp>

namespace xanthus
{
namespace component
{

struct PhysicsComponent : public Component
{
    pegasus::Particle* pParticle;
};

}
}

#endif // XANTHUS_COMPONENT_PHYSICS_COMPONENT_HPP
