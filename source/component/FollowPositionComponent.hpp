#ifndef XANTHUS_COMPONENT_FOLLOw_POSITION_COMPONENT_HPP
#define XANTHUS_COMPONENT_FOLLOw_POSITION_COMPONENT_HPP

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

struct FollowPositionComponent : public sleipnir::ecs::Component
{
    using WorldTime = sleipnir::ecs::WorldTime;

    glm::vec3 startPosition;
    sleipnir::ecs::entity::Entity target;

    WorldTime::TimeUnit startTime;
    WorldTime::TimeUnit endTime;

    system::animation::FilterFunc pFilter;

    wink::signal< wink::slot<void(sleipnir::ecs::entity::Entity)> > onFail;
    wink::signal< wink::slot<void(sleipnir::ecs::entity::Entity)> > onComplete;
};

}
}

#endif // XANTHUS_COMPONENT_FOLLOw_POSITION_COMPONENT_HPP
