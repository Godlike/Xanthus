#include "assemblage/ProjectileFactory.hpp"

#include "assemblage/Factory.hpp"

#include "component/LifetimeComponent.hpp"
#include "component/PositionComponent.hpp"
#include "component/TimerComponent.hpp"
#include "component/FollowPositionComponent.hpp"

#include "system/animation/Linear.hpp"

namespace xanthus
{
namespace assemblage
{

using namespace xanthus::component;

ProjectileFactory::ProjectileFactory(WorldTime const& worldTime, Factory& factory)
    : m_worldTime(worldTime)
    , m_factory(factory)
{

}

void ProjectileFactory::Create(entity::Entity entity, Order const& order)
{
    WorldTime::TimeUnit const now = m_worldTime.GetTime();
    WorldTime::TimeUnit const end = now + WorldTime::TimeUnit(std::chrono::seconds(1));

    // Position
    {
        component::PositionComponent& positionComponent = entity.AddComponent<component::PositionComponent>();
        positionComponent.position = order.position;
    }

    // Follow position
    {
        component::FollowPositionComponent& followComponent = entity.AddComponent<component::FollowPositionComponent>();

        followComponent.startPosition = order.position;
        followComponent.target = order.target;
        followComponent.startTime = now;
        followComponent.endTime = end;
        followComponent.pFilter = &system::animation::Linear;

        followComponent.onFail.connect(ProjectileFactory::Delete);

        followComponent.onComplete.connect(ProjectileFactory::Delete);
        followComponent.onComplete.connect(this, &ProjectileFactory::Hit);
    }

    // Timer
    {
        component::TimerComponent& timerComponent = entity.AddComponent<component::TimerComponent>();

        timerComponent.lastTime = now;
        timerComponent.endTime = end;
        timerComponent.tick = WorldTime::TimeUnit(std::chrono::milliseconds(100));

        timerComponent.onTick.connect(this, &ProjectileFactory::Tick);
    }
}

void ProjectileFactory::Delete(entity::Entity entity)
{
    LifetimeComponent& lifetimeComp = entity.AddComponent<LifetimeComponent>();
    lifetimeComp.deadline = WorldTime::TimeUnit(0);
}

void ProjectileFactory::Tick(entity::Entity entity)
{
    using ParticleEffect = Factory::Orders::ParticleEffect;

    PositionComponent const& posComp = entity.GetComponent<PositionComponent>();

    m_factory.orders.particleEffects.push(ParticleEffect{
        posComp.position
        , glm::vec3{0, 0, 0}
        , WorldTime::TimeUnit(std::chrono::milliseconds(500))
        , 2
        , ParticleEffect::Type::Down
    });
}

void ProjectileFactory::Hit(entity::Entity entity)
{
    using ParticleEffect = Factory::Orders::ParticleEffect;

    FollowPositionComponent const& animComp = entity.GetComponent<FollowPositionComponent>();
    PositionComponent const& posComp = entity.GetComponent<PositionComponent>();

    glm::vec3 path = posComp.position - animComp.startPosition;

    m_factory.orders.particleEffects.push(ParticleEffect{
        posComp.position
        , path
        , WorldTime::TimeUnit(std::chrono::seconds(1))
        , 16
        , ParticleEffect::Type::Down
    });
}

}
}
