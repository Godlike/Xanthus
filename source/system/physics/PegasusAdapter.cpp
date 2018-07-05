#include "system/physics/PegasusAdapter.hpp"

#include <Arion/Shape.hpp>

#include "util/Config.hpp"

namespace xanthus
{
namespace system
{
namespace physics
{

PegasusAdapter::PegasusAdapter()
    : primitiveCount(0)
    , m_scene()
{

}

void PegasusAdapter::Init()
{
    // Forces
    {
        double const force = 9.8;

        m_staticForces.resize(static_cast<std::size_t>(Force::Count));

        m_staticForces[static_cast<std::size_t>(Force::Down)] = std::make_unique<pegasus::scene::Force<pegasus::force::StaticField>>(m_scene, pegasus::force::StaticField(glm::dvec3{ 0, -force, 0 }));
        m_staticForces[static_cast<std::size_t>(Force::Up)] = std::make_unique<pegasus::scene::Force<pegasus::force::StaticField>>(m_scene, pegasus::force::StaticField(glm::dvec3{ 0, force, 0 }));
    }
}

pegasus::scene::Handle PegasusAdapter::SpawnBody(SpawnInfo const& info)
{
    using pegasus::scene::Primitive;

    assert(nullptr != info.pShape);

    Primitive* pPrimitive = nullptr;

    pegasus::mechanics::Body body;
    body.material.damping = info.damping;

    body.linearMotion.position = info.pShape->centerOfMass;
    body.linearMotion.velocity = info.velocity;

    Primitive::Type primitiveType = Primitive::Type::DYNAMIC;

    if (!std::isnan(info.mass))
    {
        body.material.SetMass(info.mass);
    }
    else
    {
        body.material.SetInfiniteMass();
        primitiveType = Primitive::Type::STATIC;
    }

    {
        using Type = arion::SimpleShape::Type;

        switch (info.pShape->type)
        {
            case Type::PLANE:
            {
                pPrimitive = new pegasus::scene::Plane(m_scene
                    , primitiveType
                    , body
                    , *static_cast<arion::Plane*>(info.pShape)
                );

                break;
            }
            case Type::BOX:
            {
                arion::Box& shape = *static_cast<arion::Box*>(info.pShape);

                pPrimitive = new pegasus::scene::Box(m_scene
                    , primitiveType
                    , body
                    , shape
                );

                body.material.SetMomentOfInertia(
                    pegasus::mechanics::CalculateSolidCuboidMomentOfInertia(
                        glm::length(shape.iAxis)
                        , glm::length(shape.jAxis)
                        , glm::length(shape.kAxis)
                        , body.material.GetMass()
                    )
                );

                break;
            }
            case Type::SPHERE:
            {
                arion::Sphere& shape = *static_cast<arion::Sphere*>(info.pShape);

                pPrimitive = new pegasus::scene::Sphere(m_scene
                    , primitiveType
                    , body
                    , shape
                );

                body.material.SetMomentOfInertia(
                    pegasus::mechanics::CalculateSolidSphereMomentOfInertia(
                        shape.radius
                        , body.material.GetMass()
                    )
                );

                break;
            }
            default:
            {
                return pegasus::scene::Handle();
                break;
            }
        }
    }

    primitives.push_back(pPrimitive);
    ++primitiveCount;

    if (!body.material.HasInfiniteMass())
    {
        if (Force::Count != info.force)
        {
            m_staticForces[static_cast<std::size_t>(info.force)]->Bind(*pPrimitive);
        }

        for (auto & force : m_dynamicForces)
        {
            force.second->Bind(*pPrimitive);
        }
    }

    return pPrimitive->GetBodyHandle();
}

void PegasusAdapter::PushBody(pegasus::scene::Handle bodyHandle, glm::vec3 force)
{
    assert(pegasus::scene::Handle() != bodyHandle);

    pegasus::mechanics::Body& body = m_scene.GetBody(bodyHandle);
    body.linearMotion.velocity += force;
}

void PegasusAdapter::DeleteBody(pegasus::scene::Handle bodyHandle)
{
    assert(pegasus::scene::Handle() != bodyHandle);

    auto primitiveIt = std::find_if(
        primitives.begin()
        , primitives.end()
        , [=](auto const& ptr) -> bool
        {
            return ptr->GetBodyHandle() == bodyHandle;
        }
    );

    if (primitiveIt != primitives.end())
    {
        pegasus::scene::Primitive* const pPrimitive = *primitiveIt;

        primitives.erase(primitiveIt);
        --primitiveCount;

        for (auto& forceIt : m_staticForces)
        {
            forceIt->Unbind(*pPrimitive);
        }

        delete pPrimitive;
    }
}

void PegasusAdapter::CreateGravitySource(uint32_t id, glm::vec3 position, double magnitude)
{
    m_dynamicForces[id] = std::make_unique<pegasus::scene::Force<pegasus::force::SquareDistanceSource>>(
        m_scene
        , pegasus::force::SquareDistanceSource(magnitude, position)
    );
}

void PegasusAdapter::DeleteGravitySource(uint32_t id)
{
    m_dynamicForces.erase(id);
}

void PegasusAdapter::Run(WorldTime::TimeUnit tick)
{
    m_scene.ComputeFrame(
        static_cast<float>(tick.count())
        * (static_cast<float>(decltype(tick)::period::num)
            / static_cast<float>(decltype(tick)::period::den)
        )
    );
}

}
}
}
