#include "assemblage/Factory.hpp"

#include "Application.hpp"
#include "Systems.hpp"
#include "WorldTime.hpp"

#include "entity/Entity.hpp"
#include "entity/World.hpp"

#include "component/DummyComponent.hpp"
#include "component/LifetimeComponent.hpp"
#include "component/PhysicsComponent.hpp"
#include "component/RenderComponent.hpp"
#include "component/TimerComponent.hpp"

#include "system/Render.hpp"

#include "util/Math.hpp"

#include <unicorn/video/Primitives.hpp>

#include <Arion/Shape.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <random>

namespace xanthus
{
namespace assemblage
{

Factory::Factory(WorldTime& worldTime, entity::World& world, Systems& systems)
    : m_worldTime(worldTime)
    , m_world(world)
    , m_systems(systems)
    , m_spawners(worldTime, *this, m_systems)
{
    orders.particleEffects.connect(this, &Factory::CreateParticleEffect);

    orders.projectiles.connect(this, &Factory::CreateProjectile);

    ExecuteOrders();
}

void Factory::ExecuteOrders()
{
    orders.particleEffects.emit();
    orders.projectiles.emit();
}

void Factory::ReclaimEntity(entity::Entity const& entity)
{
    if (entity.HasComponent<component::PhysicsComponent>())
    {
        component::PhysicsComponent const& physicsComponent = entity.GetComponent<component::PhysicsComponent>();
        m_systems.m_physics.DeleteBody(physicsComponent.pHandle);
    }

    if (entity.HasComponent<component::RenderComponent>())
    {
        component::RenderComponent const& renderComponent = entity.GetComponent<component::RenderComponent>();
        m_systems.m_render.DeleteMesh(renderComponent.pMesh);
        delete renderComponent.pMaterial;
    }
}

Factory::CustomSpawners::CustomSpawners(
    WorldTime& worldTime
    , Factory& factory
    , Systems& systems
)
    : projectile(worldTime, factory, systems.m_render)
{

}

entity::Entity Factory::CreateBox(glm::vec3 position, double size)
{
    entity::Entity entity = m_world.CreateEntity();

    // Position
    {
        component::PositionComponent& comp = entity.AddComponent<component::PositionComponent>();
        comp.position = position;
    }

    // Physics
    {
        component::PhysicsComponent& comp = entity.AddComponent<component::PhysicsComponent>();
        comp.pHandle = m_systems.m_physics.SpawnBody({
            new arion::Box(position
                , glm::dvec3{1, 0, 0} * (size / 2)
                , glm::dvec3{0, 1, 0} * (size / 2)
                , glm::dvec3{0, 0, 1}
            )
            , position
            , glm::dvec3(0.0)
            , std::numeric_limits<double>::quiet_NaN()
            , 0.0f
            , Force::Down
        });
    }

    // Render
    {
        using unicorn::video::Primitives;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> colorDistribution(0.45, 0.55);

        system::Render::Material* pMaterial = new system::Render::Material();
        pMaterial->color = util::math::randvec3(colorDistribution, randEngine);

        system::Render::Mesh* pMesh = m_systems.m_render.SpawnMesh(*pMaterial);
        Primitives::Box(*pMesh);
        pMesh->Scale(glm::vec3(size, size, 1.0f));

        component::RenderComponent& renderComp = entity.AddComponent<component::RenderComponent>();
        renderComp.pMesh = pMesh;
        renderComp.pMaterial = pMaterial;
    }

    return entity;
}

entity::Entity Factory::CreateSphere(double radius)
{
    entity::Entity entity = m_world.CreateEntity();

    // Position
    {
        component::PositionComponent& comp = entity.AddComponent<component::PositionComponent>();
        comp.position = glm::vec3{
            std::numeric_limits<float>::quiet_NaN()
            , std::numeric_limits<float>::quiet_NaN()
            , std::numeric_limits<float>::quiet_NaN()
        };
    }

    // Render
    {
        using unicorn::video::Primitives;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> colorDistribution(0.0, 1.0);

        system::Render::Material* pMaterial = new system::Render::Material();
        pMaterial->color = util::math::randvec3(colorDistribution, randEngine);

        system::Render::Mesh* pMesh = m_systems.m_render.SpawnMesh(*pMaterial);
        Primitives::Sphere(*pMesh, radius, 32, 32);

        component::RenderComponent& renderComp = entity.AddComponent<component::RenderComponent>();
        renderComp.pMesh = pMesh;
        renderComp.pMaterial = pMaterial;
    }

    return entity;
}

void Factory::ApplySpherePhysics(entity::Entity sphere, double radius, Orders::ParticleEffect impulse)
{
    component::PhysicsComponent& comp = sphere.AddComponent<component::PhysicsComponent>();

    Force force = Force::Down;

    switch (impulse.type)
    {
        case Orders::ParticleEffect::Type::Up:
        {
            force = Force::Up;
        }
        case Orders::ParticleEffect::Type::Down:
        default:
        {
            force = Force::Down;
            break;
        }
    }

    comp.pHandle = m_systems.m_physics.SpawnBody({
        new arion::Sphere(glm::dvec3{0, 0, 0}
            , radius
        )
        , impulse.position
        , impulse.velocity
        , (radius * radius * radius) * 4.18879020479
        , 0.9f
        , force
    });
}

void Factory::CreateParticleEffect(Orders::ParticleEffect const& order)
{
    WorldTime::TimeUnit now = m_worldTime.GetTime();

    std::random_device rd;
    std::mt19937 randEngine(rd());

    std::uniform_real_distribution<> sizeDistribution(0.1, 1.0);
    std::uniform_real_distribution<> posDistribution(-1.0f, 1.0f);
    std::uniform_real_distribution<> colorDistribution(0.0, 1.0);
    std::uniform_real_distribution<> velocityDistribution(-5.0, 5.0);

    entity::World::Entities entities = m_world.CreateEntities(order.count);

    for (uint16_t i = 0; i < order.count; ++i)
    {
        double const side = sizeDistribution(randEngine);
        glm::vec3 posShift = util::math::randvec3(posDistribution, randEngine);
        glm::vec3 pos = order.position + posShift;

        // Position
        {
            component::PositionComponent& positionComponent = entities[i].AddComponent<component::PositionComponent>();
            positionComponent.position = pos;
        }

        // Lifetime
        {
            component::LifetimeComponent& lifetimeComponent = entities[i].AddComponent<component::LifetimeComponent>();
            lifetimeComponent.deadline = now + order.ttl;
        }

        // Physics
        {
            arion::SimpleShape* pShape = nullptr;
            Force force = Force::Down;

            switch (order.type)
            {
                case Orders::ParticleEffect::Type::Up:
                {
                    pShape = new arion::Box(pos
                        , glm::dvec3{1, 0, 0} * side
                        , glm::dvec3{0, 1, 0} * side
                        , glm::dvec3{0, 0, 1} * side
                    );
                    force = Force::Up;
                }
                case Orders::ParticleEffect::Type::Down:
                default:
                {
                    pShape = new arion::Sphere(pos, side);
                    break;
                }
            }

            component::PhysicsComponent& physicsComponent = entities[i].AddComponent<component::PhysicsComponent>();
            physicsComponent.pHandle = m_systems.m_physics.SpawnBody({
                pShape
                , pos
                , util::math::randvec3(velocityDistribution, randEngine) + order.velocity
                , side
                , 0.98f
                , force
            });
        }

        // Render
        {
            using unicorn::video::Primitives;

            system::Render::Material* pMaterial = new system::Render::Material();
            pMaterial->color = util::math::randvec3(colorDistribution, randEngine);

            system::Render::Mesh* pMesh = m_systems.m_render.SpawnMesh(*pMaterial);

            switch (order.type)
            {
                case Orders::ParticleEffect::Type::Up:
                {
                    Primitives::Box(*pMesh);

                    std::vector<unicorn::video::Vertex> vertices = pMesh->GetVertices();
                    for (auto& v : vertices)
                    {
                        v.pos *= 2 * side;
                    }

                    pMesh->SetMeshData(vertices, pMesh->GetIndices());

                    break;
                }
                case Orders::ParticleEffect::Type::Down:
                default:
                {
                    Primitives::Sphere(*pMesh, side, 16, 16);
                    break;
                }
            }

            pMesh->SetTranslation(glm::vec3{
                std::numeric_limits<float>::quiet_NaN()
                , std::numeric_limits<float>::quiet_NaN()
                , std::numeric_limits<float>::quiet_NaN()
            });
            pMesh->UpdateModelMatrix();

            component::RenderComponent& renderComp = entities[i].AddComponent<component::RenderComponent>();
            renderComp.pMesh = pMesh;
            renderComp.pMaterial = pMaterial;
        }
    }
}

void Factory::CreateProjectile(Orders::Projectile const& order)
{
    m_spawners.projectile.Create(m_world.CreateEntity(), order);
}

}
}
