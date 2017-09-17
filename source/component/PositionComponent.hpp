#ifndef XANTHUS_COMPONENT_POSITION_COMPONENT_HPP
#define XANTHUS_COMPONENT_POSITION_COMPONENT_HPP

#include "util/Types.hpp"

#include <glm/glm.hpp>

namespace xanthus
{
namespace component
{

struct PositionComponent : public Component
{
    glm::vec3 position;
};

}
}

#endif // XANTHUS_COMPONENT_POSITION_COMPONENT_HPP
