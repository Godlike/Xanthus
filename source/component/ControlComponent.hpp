#ifndef XANTHUS_COMPONENT_CONTROL_COMPONENT_HPP
#define XANTHUS_COMPONENT_CONTROL_COMPONENT_HPP

#include "util/Types.hpp"

#include <glm/glm.hpp>

namespace xanthus
{
namespace component
{

struct ControlComponent : public Component
{
    glm::vec3 force;
};

}
}

#endif // XANTHUS_COMPONENT_CONTROL_COMPONENT_HPP
