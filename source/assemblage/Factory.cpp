#include "assemblage/Factory.hpp"

#include "Application.hpp"

#include "entity/Entity.hpp"
#include "entity/World.hpp"

#include "component/PhysicsComponent.hpp"
#include "component/RenderComponent.hpp"
#include "component/LifetimeComponent.hpp"

#include "system/Render.hpp"

#include <unicorn/video/Primitives.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <random>

namespace
{
    template<class Distribution, class Engine>
        glm::vec3 randvec3(Distribution& distribution, Engine& engine)
    {
        return glm::vec3{distribution(engine), distribution(engine), distribution(engine)};
    }
}

namespace xanthus
{
namespace assemblage
{

Factory::Factory(entity::World& world, Systems& systems)
    : m_world(world)
    , m_systems(systems)
{
    orders.particleEffects.connect(this, &Factory::CreateParticleEffect);
    orders.planes.connect(this, &Factory::CreatePlane);
    orders.planes.push(Factory::Orders::Plane{
        glm::vec3{1.0f, 20.0f, 0.0f},
        glm::vec3{0.0f, -1.0f, 0.0f}
    });
}

void Factory::ExecuteOrders()
{
    orders.planes.emit();
    orders.particleEffects.emit();
}

void Factory::ReclaimEntity(entity::Entity const& entity)
{
    if (entity.HasComponent<component::PhysicsComponent>())
    {
        component::PhysicsComponent const& physicsComponent = entity.GetComponent<component::PhysicsComponent>();
        m_systems.m_physics.DeleteBody(physicsComponent.pBody);
    }

    if (entity.HasComponent<component::RenderComponent>())
    {
        component::RenderComponent const& renderComponent = entity.GetComponent<component::RenderComponent>();
        m_systems.m_render.DeleteMesh(renderComponent.pMesh);
        delete renderComponent.pMaterial;
    }
}

void Factory::CreateParticleEffect(Orders::ParticleEffect const& order)
{
    entity::World::TimeUnit now = m_world.GetTime();

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
        glm::vec3 posShift = randvec3(posDistribution, randEngine);
        glm::vec3 pos = order.position + posShift;

        // Position
        {
            component::PositionComponent& positionComponent = entities[i].AddComponent<component::PositionComponent>();
            positionComponent.position = pos;
        }

        // Lifetime
        {
            component::LifetimeComponent& lifetimeComponent = entities[i].AddComponent<component::LifetimeComponent>();
            lifetimeComponent.spawned = now;
            lifetimeComponent.ttl = order.ttl;
        }

        // Physics
        {
            pegasus::geometry::SimpleShape* pShape = nullptr;
            system::Physics::Force force = system::Physics::Force::Down;

            switch (order.type)
            {
                case Orders::ParticleEffect::Type::Up:
                {
                    pShape = new pegasus::geometry::Box(pos
                        , glm::dvec3{1, 0, 0} * side
                        , glm::dvec3{0, 1, 0} * side
                        , glm::dvec3{0, 0, 1} * side
                    );
                    force = system::Physics::Force::Up;
                }
                case Orders::ParticleEffect::Type::Down:
                default:
                {
                    pShape = new pegasus::geometry::Sphere(pos, side);
                    break;
                }
            }

            component::PhysicsComponent& physicsComponent = entities[i].AddComponent<component::PhysicsComponent>();
            physicsComponent.pBody = m_systems.m_physics.SpawnBody(pShape, true, force);

            physicsComponent.pBody->s->centerOfMass = pos;
            physicsComponent.pBody->p.SetPosition(pos);
            physicsComponent.pBody->p.SetMass(side);
            physicsComponent.pBody->p.SetVelocity(randvec3(velocityDistribution, randEngine));
            physicsComponent.pBody->p.SetDamping(0.98f);
        }

        // Render
        {
            using unicorn::video::Primitives;

            system::Render::Material* pMaterial = new system::Render::Material();
            pMaterial->color = randvec3(colorDistribution, randEngine);

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

            pMesh->modelMatrix = glm::translate(glm::mat4(1), pos);

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
        physicsComponent.pBody = m_systems.m_physics.SpawnBody(pShape, false);

        physicsComponent.pBody->s->centerOfMass = order.position;
        physicsComponent.pBody->p.SetPosition(order.position);
        physicsComponent.pBody->p.SetInverseMass(0.0f);
    }

    // Render
    {
        using unicorn::video::Primitives;

        double const scale = 100.0f;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> colorDistribution(0.0, 1.0);

        system::Render::Material* pMaterial = new system::Render::Material();
        pMaterial->color = randvec3(colorDistribution, randEngine);

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

}
}
