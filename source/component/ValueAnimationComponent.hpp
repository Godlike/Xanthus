#ifndef XANTHUS_COMPONENT_VALUE_ANIMATION_COMPONENT_HPP
#define XANTHUS_COMPONENT_VALUE_ANIMATION_COMPONENT_HPP

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

struct ValueAnimationComponent : public Component
{
    glm::vec3 startPosition;
    entity::Entity target;

    WorldTime::TimeUnit startTime;
    WorldTime::TimeUnit endTime;

    system::animation::FilterFunc pFilter;

    wink::signal< wink::slot<void(entity::Entity)> > onFail;
    wink::signal< wink::slot<void(entity::Entity)> > onComplete;
};

}
}

#endif // XANTHUS_COMPONENT_VALUE_ANIMATION_COMPONENT_HPP
