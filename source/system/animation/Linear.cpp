#include "system/animation/Linear.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include <iostream>

namespace xanthus
{
namespace system
{
namespace animation
{

void Linear(
    WorldTime::TimeUnit now
    , sleipnir::ecs::component::PositionComponent& positionComponent
    , component::FollowPositionComponent& followComponent
)
{
    glm::vec3 const& targetPosition = followComponent.target.GetComponent<sleipnir::ecs::component::PositionComponent>().position;

    glm::vec3 const path = targetPosition - followComponent.startPosition;
    glm::vec3 const direction = glm::normalize(path);
    float const length = glm::length(path);

    float progress = std::min(1.0f, static_cast<float>((now - followComponent.startTime).count())
        / static_cast<float>((followComponent.endTime - followComponent.startTime).count()));

    positionComponent.position = followComponent.startPosition + direction * length * progress;
}

void Linear(
    WorldTime::TimeUnit now
    , sleipnir::ecs::component::PositionComponent& positionComponent
    , component::MoveComponent& moveComponent
)
{
    glm::vec3 const direction = glm::normalize(moveComponent.targetPosition - moveComponent.startPosition);
    float const length = (now - moveComponent.startTime).count() * moveComponent.speed;

    positionComponent.position = moveComponent.startPosition + direction * length;

    glm::vec3 const actualMove = positionComponent.position - moveComponent.startPosition;
    glm::vec3 const plannedMove = moveComponent.targetPosition - moveComponent.startPosition;

    if (glm::length2(actualMove) > glm::length2(plannedMove))
    {
        positionComponent.position = moveComponent.targetPosition;
    }
}

}
}
}
