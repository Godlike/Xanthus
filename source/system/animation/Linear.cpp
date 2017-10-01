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
    , component::ValueAnimationComponent& animationComponent
)
{
    glm::vec3 const& targetPosition = animationComponent.target.GetComponent<component::PositionComponent>().position;

    glm::vec3 path = targetPosition - animationComponent.startPosition;
    glm::vec3 direction = glm::normalize(path);
    float length = glm::length(path);

    float progress = std::min(1.0f, static_cast<float>((now - animationComponent.startTime).count())
        / static_cast<float>((animationComponent.endTime - animationComponent.startTime).count()));

    positionComponent.position = animationComponent.startPosition + direction * length * progress;
}

}
}
}
