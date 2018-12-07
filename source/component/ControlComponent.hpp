#ifndef XANTHUS_COMPONENT_CONTROL_COMPONENT_HPP
#define XANTHUS_COMPONENT_CONTROL_COMPONENT_HPP

#include <sleipnir/ecs/Types.hpp>

#include <glm/glm.hpp>

namespace xanthus
{
namespace component
{

struct ControlComponent : public sleipnir::ecs::Component
{
    glm::vec3 force;
};

}
}

#endif // XANTHUS_COMPONENT_CONTROL_COMPONENT_HPP
