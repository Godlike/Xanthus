#include "assemblage/ProjectileFactory.hpp"

#include "assemblage/Factory.hpp"

#include "component/FollowPositionComponent.hpp"

#include "system/animation/Linear.hpp"

#include "util/Math.hpp"

#include <sleipnir/ecs/component/LifetimeComponent.hpp>
#include <sleipnir/ecs/component/PositionComponent.hpp>
#include <sleipnir/ecs/component/TimerComponent.hpp>
#include <sleipnir/ecs/Systems.hpp>

#include <unicorn/video/Primitives.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <random>

namespace xanthus
{
namespace assemblage
{

ProjectileFactory::ProjectileFactory(sleipnir::SleipnirEngine& engine
    , assemblage::Factory& factory
)
    : m_worldTime(engine.GetWorldTime())
    , m_factory(factory)
    , m_render(engine.GetSystems().GetRender())
{

}

void ProjectileFactory::Create(sleipnir::ecs::entity::Entity entity, Order const& order)
{
    WorldTime::TimeUnit const now = m_worldTime.GetTime();
    WorldTime::TimeUnit const end = now + WorldTime::TimeUnit(std::chrono::seconds(1));

    // Position
    {
        sleipnir::ecs::component::PositionComponent& positionComponent = entity.AddComponent<sleipnir::ecs::component::PositionComponent>();
        positionComponent.position = order.position;
    }

    // Render
    {
        using unicorn::video::Primitives;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> sizeDistribution(2.0, 7.0);
        std::uniform_real_distribution<> colorDistribution(0.0, 1.0);

        sleipnir::ecs::system::Render::Material* pMaterial = new sleipnir::ecs::system::Render::Material();
        pMaterial->SetColor(util::math::randvec3(colorDistribution, randEngine));

        sleipnir::ecs::system::Render::Mesh* pMesh = m_render.SpawnMesh();
        Primitives::Sphere(*pMesh, sizeDistribution(randEngine), 16, 16);

        //! @todo   remove the following call when Unicorn#121 is resolved
        const_cast<glm::mat4&>(pMesh->GetModelMatrix()) = glm::mat4(1.0f);
        pMesh->SetTranslation(order.position);
        pMesh->UpdateTransformMatrix();

        pMesh->SetMaterial(std::shared_ptr<sleipnir::ecs::system::Render::Material>(pMaterial));

        sleipnir::ecs::component::RenderComponent& renderComp = entity.AddComponent<sleipnir::ecs::component::RenderComponent>();
        renderComp.pMesh = pMesh;
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

        followComponent.onComplete = order.onComplete;
        followComponent.onComplete.connect(ProjectileFactory::Delete);
        followComponent.onComplete.connect(this, &ProjectileFactory::Hit);
    }

    // Timer
    {
        sleipnir::ecs::component::TimerComponent& timerComponent = entity.AddComponent<sleipnir::ecs::component::TimerComponent>();

        timerComponent.lastTime = now;
        timerComponent.endTime = end;
        timerComponent.tick = WorldTime::TimeUnit(std::chrono::milliseconds(100));

        timerComponent.onTick.connect(this, &ProjectileFactory::Tick);
    }
}

void ProjectileFactory::Delete(sleipnir::ecs::entity::Entity entity)
{
    entity.AddComponent<sleipnir::ecs::component::LifetimeComponent>();
}

void ProjectileFactory::Tick(sleipnir::ecs::entity::Entity entity)
{
    using ParticleEffect = Factory::Orders::ParticleEffect;

    sleipnir::ecs::component::PositionComponent const& posComp = entity.GetComponent<sleipnir::ecs::component::PositionComponent>();

    m_factory.orders.particleEffects.push(ParticleEffect{
        posComp.position
        , glm::vec3{0, 0, 0}
        , WorldTime::TimeUnit(std::chrono::milliseconds(500))
        , 2
        , ParticleEffect::Type::Down
    });
}

void ProjectileFactory::Hit(sleipnir::ecs::entity::Entity entity)
{
    using ParticleEffect = Factory::Orders::ParticleEffect;

    component::FollowPositionComponent const& animComp = entity.GetComponent<component::FollowPositionComponent>();
    sleipnir::ecs::component::PositionComponent const& posComp = entity.GetComponent<sleipnir::ecs::component::PositionComponent>();

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
