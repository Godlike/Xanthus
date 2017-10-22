#ifndef XANTHUS_COMPONENT_ENTITY_SNAP_COMPONENT_HPP
#define XANTHUS_COMPONENT_ENTITY_SNAP_COMPONENT_HPP

#include "entity/Entity.hpp"

#include "util/Types.hpp"

#include <glm/glm.hpp>

namespace xanthus
{
namespace component
{

struct EntitySnapComponent : public Component
{
    entity::Entity entity;
    glm::vec3 offset;
};

}
}

#endif // XANTHUS_COMPONENT_ENTITY_SNAP_COMPONENT_HPP
