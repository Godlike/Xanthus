#ifndef XANTHUS_COMPONENT_MOVE_COMPONENT_HPP
#define XANTHUS_COMPONENT_MOVE_COMPONENT_HPP

#include "WorldTime.hpp"

#include "entity/Entity.hpp"

#include "system/animation/Filter.hpp"

#include "util/Types.hpp"

#include <glm/glm.hpp>
#include <wink/signal.hpp>

namespace xanthus
{
namespace component
{

struct MoveComponent : public Component
{
    entity::Entity entity;
    glm::vec3 startPosition;
    glm::vec3 targetPosition;

    WorldTime::TimeUnit startTime;
    float speed;

    system::animation::MoveFilterFunc pFilter;

    wink::signal< wink::slot<void(entity::Entity)> > onComplete;
};

}
}

#endif // XANTHUS_COMPONENT_MOVE_COMPONENT_HPP
