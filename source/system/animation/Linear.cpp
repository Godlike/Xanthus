#include "system/animation/Linear.hpp"

#include <iostream>

namespace xanthus
{
namespace system
{
namespace animation
{

void Linear(
    WorldTime::TimeUnit now
    , component::PositionComponent& positionComponent
    , component::FollowPositionComponent& followComponent
)
{
    glm::vec3 const& targetPosition = followComponent.target.GetComponent<component::PositionComponent>().position;

    glm::vec3 path = targetPosition - followComponent.startPosition;
    glm::vec3 direction = glm::normalize(path);
    float length = glm::length(path);

    float progress = std::min(1.0f, static_cast<float>((now - followComponent.startTime).count())
        / static_cast<float>((followComponent.endTime - followComponent.startTime).count()));

    positionComponent.position = followComponent.startPosition + direction * length * progress;
}

}
}
}
