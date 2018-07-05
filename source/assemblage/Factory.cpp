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
        m_systems.m_physics.DeleteGravitySource(static_cast<uint32_t>(entity.GetId()));
        m_systems.m_physics.DeleteBody(physicsComponent.pHandle);
    }

    if (entity.HasComponent<component::RenderComponent>())
    {
        component::RenderComponent const& renderComponent = entity.GetComponent<component::RenderComponent>();
        m_systems.m_render.DeleteMesh(renderComponent.pMesh);
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

entity::Entity Factory::CreateBox(arion::Box const& box, std::uniform_real_distribution<> colorDistribution, double mass, bool physics)
{
    entity::Entity entity = m_world.CreateEntity();

    // Position
    {
        component::PositionComponent& comp = entity.AddComponent<component::PositionComponent>();
        comp.position = box.centerOfMass;
    }

    // Physics
    if (physics)
    {
        component::PhysicsComponent& comp = entity.AddComponent<component::PhysicsComponent>();
        comp.pHandle = m_systems.m_physics.SpawnBody({
            new arion::Box(box)
            , glm::dvec3(0.0)
            , mass
            , 0.2f
            , Force::Down
        });
    }

    // Render
    {
        using unicorn::video::Primitives;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        system::Render::Material* pMaterial = new system::Render::Material();
        pMaterial->SetColor(util::math::randvec3(colorDistribution, randEngine));

        system::Render::Mesh* pMesh = m_systems.m_render.SpawnMesh();
        Primitives::Box(*pMesh);

        pMesh->Scale(glm::vec3(
            glm::length(box.iAxis) * 2.0f
            , glm::length(box.jAxis) * 2.0f
            , glm::length(box.kAxis) * 2.0f
        )); // scale to desired size

        pMesh->SetMaterial(std::shared_ptr<system::Render::Material>(pMaterial));

        //! @todo   remove the following call when Unicorn#120 is resolved
        m_systems.m_render.AddMesh(pMesh);

        component::RenderComponent& renderComp = entity.AddComponent<component::RenderComponent>();
        renderComp.pMesh = pMesh;
    }

    return entity;
}

entity::Entity Factory::CreateSphere(arion::Sphere const& sphere)
{
    WorldTime::TimeUnit now = m_worldTime.GetTime();

    entity::Entity entity = m_world.CreateEntity();

    // Position
    {
        component::PositionComponent& comp = entity.AddComponent<component::PositionComponent>();
        comp.position = sphere.centerOfMass;
    }

    // Lifetime
    {
        component::LifetimeComponent& lifetimeComponent = entity.AddComponent<component::LifetimeComponent>();
        lifetimeComponent.deadline = now + std::chrono::seconds(10);
    }

    // Render
    {
        using unicorn::video::Primitives;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> colorDistribution(0.0, 1.0);

        system::Render::Material* pMaterial = new system::Render::Material();
        pMaterial->SetColor(util::math::randvec3(colorDistribution, randEngine));

        system::Render::Mesh* pMesh = m_systems.m_render.SpawnMesh();
        Primitives::Sphere(*pMesh, sphere.radius, 32, 32);

        pMesh->SetMaterial(std::shared_ptr<system::Render::Material>(pMaterial));

        //! @todo   remove the following call when Unicorn#120 is resolved
        m_systems.m_render.AddMesh(pMesh);

        component::RenderComponent& renderComp = entity.AddComponent<component::RenderComponent>();
        renderComp.pMesh = pMesh;
    }

    return entity;
}

entity::Entity Factory::CreatePlane(arion::Plane const& plane)
{
    entity::Entity entity = m_world.CreateEntity();

    // Position
    {
        component::PositionComponent& comp = entity.AddComponent<component::PositionComponent>();
        comp.position = plane.centerOfMass;
    }

    // Physics
    {
        component::PhysicsComponent& comp = entity.AddComponent<component::PhysicsComponent>();
        comp.pHandle = m_systems.m_physics.SpawnBody({
            new arion::Plane(plane)
            , glm::dvec3(0.0)
            , std::numeric_limits<double>::quiet_NaN()
            , 0.0f
            , Force::Down
        });
    }

    // Render
    {
        using unicorn::video::Primitives;

        static constexpr float const planeScale = 1000.0f;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> colorDistribution(0.8, 1.0);

        system::Render::Material* pMaterial = new system::Render::Material();
        pMaterial->SetColor(util::math::randvec3(colorDistribution, randEngine));

        system::Render::Mesh* pMesh = m_systems.m_render.SpawnMesh();
        Primitives::Quad(*pMesh);

        pMesh->Scale(glm::vec3(planeScale, planeScale, 0.0f));
        pMesh->Rotate(
            std::acos(glm::dot(glm::dvec3(0, 0, 1), plane.normal))
            , glm::cross(glm::dvec3(0, 0, 1), plane.normal)
        );

        pMesh->SetMaterial(std::shared_ptr<system::Render::Material>(pMaterial));

        //! @todo   remove the following call when Unicorn#120 is resolved
        m_systems.m_render.AddMesh(pMesh);

        component::RenderComponent& renderComp = entity.AddComponent<component::RenderComponent>();
        renderComp.pMesh = pMesh;
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
        new arion::Sphere(impulse.position
            , glm::quat()
            , radius
        )
        , impulse.velocity
        , (radius * radius * radius) * 4.18879020479
        , 0.9f
        , force
    });
}

void Factory::ApplyGravitySource(entity::Entity sphere, double radius, double magnitude)
{
    if (sphere.HasComponent<component::LifetimeComponent>())
    {
        sphere.DeleteComponent<component::LifetimeComponent>();
    }

    component::PositionComponent& posComp = sphere.GetComponent<component::PositionComponent>();
    component::PhysicsComponent& comp = sphere.AddComponent<component::PhysicsComponent>();

    comp.pHandle = m_systems.m_physics.SpawnBody({
        new arion::Sphere(posComp.position
            , glm::quat()
            , radius
        )
        , glm::dvec3(0)
        , std::numeric_limits<double>::quiet_NaN()
        , 1.0
        , Force::Count
    });

    {
        component::RenderComponent& renderComp = sphere.GetComponent<component::RenderComponent>();

        glm::vec3 color(0);
        static constexpr double const maxMagnitude = 100.0;

        if (magnitude > 0)
        {
            color = glm::vec3(1.0f, 0.38f, 0.118f) * static_cast<float>(magnitude / maxMagnitude);
        }
        else if (magnitude < 0)
        {
            color = glm::vec3(0.47f, 0.82f, 1.0f) * static_cast<float>((-magnitude) / maxMagnitude);
        }

        renderComp.pMesh->GetMaterial()->SetColor(color);
    }

    if (0 != magnitude)
    {
        m_systems.m_physics.CreateGravitySource(
            static_cast<uint32_t>(sphere.GetId())
            , posComp.position
            , magnitude
        );
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
            arion::SimpleShape* pShape = nullptr;
            Force force = Force::Down;

            switch (order.type)
            {
                case Orders::ParticleEffect::Type::Up:
                {
                    pShape = new arion::Box(pos
                        , glm::quat()
                        , glm::dvec3{1, 0, 0} * side
                        , glm::dvec3{0, 1, 0} * side
                        , glm::dvec3{0, 0, 1} * side
                    );
                    force = Force::Up;
                }
                case Orders::ParticleEffect::Type::Down:
                default:
                {
                    pShape = new arion::Sphere(pos, glm::quat(), side);
                    break;
                }
            }

            component::PhysicsComponent& physicsComponent = entities[i].AddComponent<component::PhysicsComponent>();
            physicsComponent.pHandle = m_systems.m_physics.SpawnBody({
                pShape
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
            pMaterial->SetColor(util::math::randvec3(colorDistribution, randEngine));

            system::Render::Mesh* pMesh = m_systems.m_render.SpawnMesh();

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

            //! @todo   remove the following call when Unicorn#121 is resolved
            const_cast<glm::mat4&>(pMesh->GetModelMatrix()) = glm::mat4(1.0f);
            pMesh->SetTranslation(glm::vec3{
                std::numeric_limits<float>::quiet_NaN()
                , std::numeric_limits<float>::quiet_NaN()
                , std::numeric_limits<float>::quiet_NaN()
            });
            pMesh->UpdateTransformMatrix();

            pMesh->SetMaterial(std::shared_ptr<system::Render::Material>(pMaterial));

            //! @todo   remove the following call when Unicorn#120 is resolved
            m_systems.m_render.AddMesh(pMesh);

            component::RenderComponent& renderComp = entities[i].AddComponent<component::RenderComponent>();
            renderComp.pMesh = pMesh;
        }
    }
}

void Factory::CreateProjectile(Orders::Projectile const& order)
{
    m_spawners.projectile.Create(m_world.CreateEntity(), order);
}

}
}
