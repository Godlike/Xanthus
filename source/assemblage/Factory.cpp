#include "assemblage/Factory.hpp"

#include "Application.hpp"
#include "Systems.hpp"

#include "util/Math.hpp"
#include "util/Types.hpp"

#include <sleipnir/ecs/component/LifetimeComponent.hpp>
#include <sleipnir/ecs/component/PhysicsComponent.hpp>
#include <sleipnir/ecs/component/RenderComponent.hpp>
#include <sleipnir/ecs/component/TimerComponent.hpp>
#include <sleipnir/ecs/entity/Entity.hpp>
#include <sleipnir/ecs/entity/World.hpp>
#include <sleipnir/ecs/WorldTime.hpp>

#include <unicorn/video/Primitives.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace xanthus
{
namespace assemblage
{

Factory::Factory(sleipnir::SleipnirEngine& engine)
    : m_worldTime(engine.GetWorldTime())
    , m_world(engine.GetEntityWorld())
    , m_render(engine.GetSystems().GetRender())
    , m_physicsBodyChanges(engine.GetSystems().GetPhysics().CloneBodyChanges())
    , m_spawners(engine, *this)
{
    orders.particleEffects.connect(this, &Factory::CreateParticleEffect);

    orders.projectiles.connect(this, &Factory::CreateProjectile);

    ExecuteOrders();
}

void Factory::ExecuteOrders()
{
    orders.particleEffects.emit();
    orders.projectiles.emit();

    m_physicsBodyChanges.Push(m_worldTime.GetTime());
}

void Factory::ReclaimEntity(sleipnir::ecs::entity::Entity const& entity)
{
    if (entity.HasComponent<sleipnir::ecs::component::PhysicsComponent>())
    {
        sleipnir::ecs::component::PhysicsComponent const& physicsComponent = entity.GetComponent<sleipnir::ecs::component::PhysicsComponent>();
        // m_physics.DeleteGravitySource(static_cast<uint32_t>(entity.GetId()));
        // m_physics.DeleteBody(physicsComponent.pHandle);
        m_physicsBodyChanges.Delete(physicsComponent.pHandle);
    }

    if (entity.HasComponent<sleipnir::ecs::component::RenderComponent>())
    {
        sleipnir::ecs::component::RenderComponent const& renderComponent = entity.GetComponent<sleipnir::ecs::component::RenderComponent>();
        m_render.DeleteMesh(renderComponent.pMesh);
    }
}

Factory::CustomSpawners::CustomSpawners(
    sleipnir::SleipnirEngine& engine
    , Factory& factory
)
    : projectile(engine, factory)
{

}

sleipnir::ecs::entity::Entity Factory::CreateBox(arion::Box const& box, std::uniform_real_distribution<> colorDistribution, double mass, bool physics)
{
    sleipnir::ecs::entity::Entity entity = m_world.CreateEntity();

    // Position
    {
        sleipnir::ecs::component::PositionComponent& comp = entity.AddComponent<sleipnir::ecs::component::PositionComponent>();
        comp.position = box.centerOfMass;
    }

    // Physics
    if (physics)
    {
        sleipnir::ecs::component::PhysicsComponent& comp = entity.AddComponent<sleipnir::ecs::component::PhysicsComponent>();

        sleipnir::ecs::system::physics::BodyMemento memento;
        memento.pShape = new arion::Box(box);

        sleipnir::ecs::system::physics::BodyMemento::LinearMotion linear;
        linear.position = box.centerOfMass;

        memento.linear = {true, linear};
        memento.mass = {true, mass};
        memento.damping = {true, 0.2f};

        comp.pHandle = m_physicsBodyChanges.Add(memento);
    }

    // Render
    {
        using unicorn::video::Primitives;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        sleipnir::ecs::system::Render::Material* pMaterial = new sleipnir::ecs::system::Render::Material();
        pMaterial->SetColor(util::math::randvec3(colorDistribution, randEngine));

        sleipnir::ecs::system::Render::Mesh* pMesh = m_render.SpawnMesh();
        Primitives::Box(*pMesh);

        pMesh->Scale(glm::vec3(
            glm::length(box.iAxis) * 2.0f
            , glm::length(box.jAxis) * 2.0f
            , glm::length(box.kAxis) * 2.0f
        )); // scale to desired size

        pMesh->SetMaterial(std::shared_ptr<sleipnir::ecs::system::Render::Material>(pMaterial));

        //! @todo   remove the following call when Unicorn#120 is resolved
        m_render.AddMesh(pMesh);

        sleipnir::ecs::component::RenderComponent& renderComp = entity.AddComponent<sleipnir::ecs::component::RenderComponent>();
        renderComp.pMesh = pMesh;
    }

    return entity;
}

sleipnir::ecs::entity::Entity Factory::CreateSphere(arion::Sphere const& sphere)
{
    WorldTime::TimeUnit now = m_worldTime.GetTime();

    sleipnir::ecs::entity::Entity entity = m_world.CreateEntity();

    // Position
    {
        sleipnir::ecs::component::PositionComponent& comp = entity.AddComponent<sleipnir::ecs::component::PositionComponent>();
        comp.position = sphere.centerOfMass;
    }

    // Lifetime
    {
        sleipnir::ecs::component::LifetimeComponent& lifetimeComponent = entity.AddComponent<sleipnir::ecs::component::LifetimeComponent>();
        lifetimeComponent.deadline = now + std::chrono::seconds(10);
    }

    // Render
    {
        using unicorn::video::Primitives;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> colorDistribution(0.0, 1.0);

        sleipnir::ecs::system::Render::Material* pMaterial = new sleipnir::ecs::system::Render::Material();
        pMaterial->SetColor(util::math::randvec3(colorDistribution, randEngine));

        sleipnir::ecs::system::Render::Mesh* pMesh = m_render.SpawnMesh();
        Primitives::Sphere(*pMesh, sphere.radius, 32, 32);

        pMesh->SetMaterial(std::shared_ptr<sleipnir::ecs::system::Render::Material>(pMaterial));

        //! @todo   remove the following call when Unicorn#120 is resolved
        m_render.AddMesh(pMesh);

        sleipnir::ecs::component::RenderComponent& renderComp = entity.AddComponent<sleipnir::ecs::component::RenderComponent>();
        renderComp.pMesh = pMesh;
    }

    return entity;
}

sleipnir::ecs::entity::Entity Factory::CreatePlane(arion::Plane const& plane)
{
    sleipnir::ecs::entity::Entity entity = m_world.CreateEntity();

    // Position
    {
        sleipnir::ecs::component::PositionComponent& comp = entity.AddComponent<sleipnir::ecs::component::PositionComponent>();
        comp.position = plane.centerOfMass;
    }

    // Physics
    {
        sleipnir::ecs::component::PhysicsComponent& comp = entity.AddComponent<sleipnir::ecs::component::PhysicsComponent>();

        sleipnir::ecs::system::physics::BodyMemento memento;
        memento.pShape = new arion::Plane(plane);

        sleipnir::ecs::system::physics::BodyMemento::LinearMotion linear;
        linear.position = plane.centerOfMass;

        memento.linear = {true, linear};
        memento.mass = {true, std::numeric_limits<double>::quiet_NaN()};
        memento.damping = {true, 0.0f};

        comp.pHandle = m_physicsBodyChanges.Add(memento);
    }

    // Render
    {
        using unicorn::video::Primitives;

        static constexpr float const planeScale = 1000.0f;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> colorDistribution(0.8, 1.0);

        sleipnir::ecs::system::Render::Material* pMaterial = new sleipnir::ecs::system::Render::Material();
        pMaterial->SetColor(util::math::randvec3(colorDistribution, randEngine));

        sleipnir::ecs::system::Render::Mesh* pMesh = m_render.SpawnMesh();
        Primitives::Quad(*pMesh);

        pMesh->Scale(glm::vec3(planeScale, planeScale, 0.0f));
        pMesh->Rotate(
            std::acos(glm::dot(glm::vec3(0, 0, 1), plane.normal))
            , glm::cross(glm::vec3(0, 0, 1), plane.normal)
        );

        pMesh->SetMaterial(std::shared_ptr<sleipnir::ecs::system::Render::Material>(pMaterial));

        //! @todo   remove the following call when Unicorn#120 is resolved
        m_render.AddMesh(pMesh);

        sleipnir::ecs::component::RenderComponent& renderComp = entity.AddComponent<sleipnir::ecs::component::RenderComponent>();
        renderComp.pMesh = pMesh;
    }

    return entity;
}

void Factory::ApplySpherePhysics(sleipnir::ecs::entity::Entity sphere, double radius, Orders::ParticleEffect impulse)
{
    sleipnir::ecs::component::PhysicsComponent& comp = sphere.AddComponent<sleipnir::ecs::component::PhysicsComponent>();

    // Force force = Force::Down;

    switch (impulse.type)
    {
        case Orders::ParticleEffect::Type::Up:
        {
            // force = Force::Up;
            break;
        }
        case Orders::ParticleEffect::Type::Down:
        default:
        {
            // force = Force::Down;
            break;
        }
    }

    sleipnir::ecs::system::physics::BodyMemento memento;
    memento.pShape = new arion::Sphere(impulse.position
            , glm::quat()
            , radius
        );

    sleipnir::ecs::system::physics::BodyMemento::LinearMotion linear;
    linear.velocity = impulse.velocity;

    memento.linear = {true, linear};
    memento.mass = {true, (radius * radius * radius) * 4.18879020479};
    memento.damping = {true, 0.9f};

    comp.pHandle = m_physicsBodyChanges.Add(memento);
}

void Factory::ApplyGravitySource(sleipnir::ecs::entity::Entity sphere, double radius, double magnitude)
{
    if (sphere.HasComponent<sleipnir::ecs::component::LifetimeComponent>())
    {
        sphere.DeleteComponent<sleipnir::ecs::component::LifetimeComponent>();
    }

    sleipnir::ecs::component::PositionComponent& posComp = sphere.GetComponent<sleipnir::ecs::component::PositionComponent>();
    sleipnir::ecs::component::PhysicsComponent& comp = sphere.AddComponent<sleipnir::ecs::component::PhysicsComponent>();

    sleipnir::ecs::system::physics::BodyMemento memento;
    memento.pShape = new arion::Sphere(posComp.position
            , glm::quat()
            , radius
        );

    sleipnir::ecs::system::physics::BodyMemento::LinearMotion linear;
    linear.position = posComp.position;

    memento.linear = {true, linear};
    memento.mass = {true, std::numeric_limits<double>::quiet_NaN()};
    memento.damping = {true, 1.0f};

    comp.pHandle = m_physicsBodyChanges.Add(memento);

    {
        sleipnir::ecs::component::RenderComponent& renderComp = sphere.GetComponent<sleipnir::ecs::component::RenderComponent>();

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
        // m_physics.CreateGravitySource(
        //     static_cast<uint32_t>(sphere.GetId())
        //     , posComp.position
        //     , magnitude
        // );
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

    sleipnir::ecs::entity::World::Entities entities = m_world.CreateEntities(order.count);

    for (uint16_t i = 0; i < order.count; ++i)
    {
        float const side = sizeDistribution(randEngine);
        glm::vec3 posShift = util::math::randvec3(posDistribution, randEngine);
        glm::vec3 pos = order.position + posShift;

        // Position
        {
            sleipnir::ecs::component::PositionComponent& positionComponent = entities[i].AddComponent<sleipnir::ecs::component::PositionComponent>();
            positionComponent.position = pos;
        }

        // Lifetime
        {
            sleipnir::ecs::component::LifetimeComponent& lifetimeComponent = entities[i].AddComponent<sleipnir::ecs::component::LifetimeComponent>();
            lifetimeComponent.deadline = now + order.ttl;
        }

        // Physics
        {
            arion::SimpleShape* pShape = nullptr;
            // Force force = Force::Down;

            switch (order.type)
            {
                case Orders::ParticleEffect::Type::Up:
                {
                    pShape = new arion::Box(pos
                        , glm::quat()
                        , glm::vec3{1, 0, 0} * side
                        , glm::vec3{0, 1, 0} * side
                        , glm::vec3{0, 0, 1} * side
                    );
                    // force = Force::Up;
                }
                case Orders::ParticleEffect::Type::Down:
                default:
                {
                    pShape = new arion::Sphere(pos, glm::quat(), side);
                    break;
                }
            }

            sleipnir::ecs::component::PhysicsComponent& physicsComponent = entities[i].AddComponent<sleipnir::ecs::component::PhysicsComponent>();

            sleipnir::ecs::system::physics::BodyMemento memento;
            memento.pShape = pShape;

            sleipnir::ecs::system::physics::BodyMemento::LinearMotion linear;
            linear.velocity = util::math::randvec3(velocityDistribution, randEngine) + order.velocity;

            memento.linear = {true, linear};
            memento.mass = {true, side};
            memento.damping = {true, 0.98f};

            physicsComponent.pHandle = m_physicsBodyChanges.Add(memento);
        }

        // Render
        {
            using unicorn::video::Primitives;

            sleipnir::ecs::system::Render::Material* pMaterial = new sleipnir::ecs::system::Render::Material();
            pMaterial->SetColor(util::math::randvec3(colorDistribution, randEngine));

            sleipnir::ecs::system::Render::Mesh* pMesh = m_render.SpawnMesh();

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

            pMesh->SetMaterial(std::shared_ptr<sleipnir::ecs::system::Render::Material>(pMaterial));

            //! @todo   remove the following call when Unicorn#120 is resolved
            m_render.AddMesh(pMesh);

            sleipnir::ecs::component::RenderComponent& renderComp = entities[i].AddComponent<sleipnir::ecs::component::RenderComponent>();
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
