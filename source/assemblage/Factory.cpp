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
    orders.dummies.connect(this, &Factory::CreateDummy);

    {
        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> posDistribution(-30.0f, 30.0f);

        for (int i = 0; i < 10; ++i)
        {
            orders.dummies.push(Factory::Orders::Dummy{
                util::math::randvec3(posDistribution, randEngine)
            });
        }
    }

    orders.particleEffects.connect(this, &Factory::CreateParticleEffect);

    orders.planes.connect(this, &Factory::CreatePlane);
    orders.planes.push(Factory::Orders::Plane{
        glm::vec3{1.0f, 20.0f, 0.0f},
        glm::vec3{0.0f, -1.0f, 0.0f}
    });

    orders.projectiles.connect(this, &Factory::CreateProjectile);

    ExecuteOrders();
}

void Factory::ExecuteOrders()
{
    orders.dummies.emit();
    orders.planes.emit();
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
    , gridplate(systems.m_render)
{

}

void Factory::CreateDummy(Orders::Dummy const& order)
{
    entity::Entity entity = m_world.CreateEntity();

    // Dummy
    {
        entity.AddComponent<component::DummyComponent>();
    }

    // Position
    {
        component::PositionComponent& positionComponent = entity.AddComponent<component::PositionComponent>();
        positionComponent.position = order.position;
    }

    // Render
    {
        using unicorn::video::Primitives;

        double const scale = 5.0f;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> colorDistribution(0.0, 1.0);

        system::Render::Material* pMaterial = new system::Render::Material();
        pMaterial->color = util::math::randvec3(colorDistribution, randEngine);

        system::Render::Mesh* pMesh = m_systems.m_render.SpawnMesh(*pMaterial);
        Primitives::Quad(*pMesh);

        std::vector<unicorn::video::Vertex> vertices = pMesh->GetVertices();
        for (auto& v : vertices)
        {
            v.pos *= scale;
        }

        pMesh->SetMeshData(vertices, pMesh->GetIndices());

        component::RenderComponent& renderComp = entity.AddComponent<component::RenderComponent>();
        renderComp.pMesh = pMesh;
        renderComp.pMaterial = pMaterial;

        pMesh->modelMatrix = glm::translate(glm::mat4(1), order.position);
    }
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
            pegasus::geometry::SimpleShape* pShape = nullptr;
            Force force = Force::Down;

            switch (order.type)
            {
                case Orders::ParticleEffect::Type::Up:
                {
                    pShape = new pegasus::geometry::Box(pos
                        , glm::dvec3{1, 0, 0} * side
                        , glm::dvec3{0, 1, 0} * side
                        , glm::dvec3{0, 0, 1} * side
                    );
                    force = Force::Up;
                }
                case Orders::ParticleEffect::Type::Down:
                default:
                {
                    pShape = new pegasus::geometry::Sphere(pos, side);
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
                , true
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
                    Primitives::Cube(*pMesh);

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

            pMesh->modelMatrix = glm::translate(glm::mat4(1), glm::vec3{
                std::numeric_limits<double>::quiet_NaN()
                , std::numeric_limits<double>::quiet_NaN()
                , std::numeric_limits<double>::quiet_NaN()
            });

            component::RenderComponent& renderComp = entities[i].AddComponent<component::RenderComponent>();
            renderComp.pMesh = pMesh;
            renderComp.pMaterial = pMaterial;
        }
    }
}

void Factory::CreatePlane(Orders::Plane const& order)
{
    entity::Entity entity = m_world.CreateEntity();

    // Position
    {
        component::PositionComponent& positionComponent = entity.AddComponent<component::PositionComponent>();
        positionComponent.position = order.position;
    }

    // Physics
    {
        pegasus::geometry::Plane* pShape = new pegasus::geometry::Plane(
            order.position, glm::normalize(order.normal)
        );

        component::PhysicsComponent& physicsComponent = entity.AddComponent<component::PhysicsComponent>();
        physicsComponent.pHandle = m_systems.m_physics.SpawnBody({
            pShape
            , order.position
            , glm::dvec3{0}
            , std::numeric_limits<double>::quiet_NaN()
            , 0.98f
            , false
            , Force::Down
        });
    }

    // Render
    {
        using unicorn::video::Primitives;

        double const scale = 100.0f;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> colorDistribution(0.0, 1.0);

        system::Render::Material* pMaterial = new system::Render::Material();
        pMaterial->color = util::math::randvec3(colorDistribution, randEngine);

        system::Render::Mesh* pMesh = m_systems.m_render.SpawnMesh(*pMaterial);
        Primitives::Quad(*pMesh);

        std::vector<unicorn::video::Vertex> vertices = pMesh->GetVertices();
        for (auto& v : vertices)
        {
            v.pos *= scale;
        }

        pMesh->SetMeshData(vertices, pMesh->GetIndices());

        component::RenderComponent& renderComp = entity.AddComponent<component::RenderComponent>();
        renderComp.pMesh = pMesh;
        renderComp.pMaterial = pMaterial;

        renderComp.rotateAngle = std::acos(glm::dot(glm::vec3(0, 0, 1), order.normal));
        renderComp.rotateAxes = glm::cross(glm::vec3(0, 0, 1), order.normal);

        pMesh->modelMatrix = glm::translate(glm::mat4(1), order.position);
        if (renderComp.rotateAngle != 0.0f)
        {
            pMesh->modelMatrix = glm::rotate(renderComp.pMesh->modelMatrix, renderComp.rotateAngle, renderComp.rotateAxes);
        }
    }
}

void Factory::CreateProjectile(Orders::Projectile const& order)
{
    m_spawners.projectile.Create(m_world.CreateEntity(), order);
}

}
}
