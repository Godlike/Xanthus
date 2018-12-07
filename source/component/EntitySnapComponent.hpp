#ifndef XANTHUS_COMPONENT_ENTITY_SNAP_COMPONENT_HPP
#define XANTHUS_COMPONENT_ENTITY_SNAP_COMPONENT_HPP

#include <sleipnir/ecs/entity/Entity.hpp>
#include <sleipnir/ecs/Types.hpp>

#include <glm/glm.hpp>

namespace xanthus
{
namespace component
{

struct EntitySnapComponent : public sleipnir::ecs::Component
{
    sleipnir::ecs::entity::Entity entity;
    glm::vec3 offset;
};

}
}

#endif // XANTHUS_COMPONENT_ENTITY_SNAP_COMPONENT_HPP
