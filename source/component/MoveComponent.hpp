#ifndef XANTHUS_COMPONENT_MOVE_COMPONENT_HPP
#define XANTHUS_COMPONENT_MOVE_COMPONENT_HPP

#include "system/animation/Filter.hpp"

#include <sleipnir/ecs/entity/Entity.hpp>
#include <sleipnir/ecs/Types.hpp>
#include <sleipnir/ecs/WorldTime.hpp>

#include <glm/glm.hpp>
#include <wink/signal.hpp>

namespace xanthus
{
namespace component
{

struct MoveComponent : public sleipnir::ecs::Component
{
    using WorldTime = sleipnir::ecs::WorldTime;

    sleipnir::ecs::entity::Entity entity;

    glm::vec3 startPosition;
    glm::vec3 targetPosition;

    WorldTime::TimeUnit startTime;
    float speed;

    system::animation::MoveFilterFunc pFilter;

    wink::signal< wink::slot<void(sleipnir::ecs::entity::Entity)> > onComplete;
};

}
}

#endif // XANTHUS_COMPONENT_MOVE_COMPONENT_HPP
